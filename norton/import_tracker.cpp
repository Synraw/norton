#include "import_tracker.h"
#include "win.h"
#include "misc.h"
#include "api.h"

namespace norton {
	void import_tracker::setup_process(process *proc) {
		m_target_process = proc;

		auto modules = proc->list_modules();
		for (auto module : modules) {
			remote_module entry;
			entry.m_name = module.m_name;
			entry.m_base = module.m_base;
			entry.m_exports = get_remote_exports(module.m_base);

			m_modules[module.m_name] = entry;
		}

		for (auto& entry : m_modules) {
			for (auto& ittr : entry.second.m_exports) {
				auto &exp = ittr.second;

				if (exp.m_is_forwarded && exp.m_address == 0) {
					auto dot		   = exp.m_forward_string.find_last_of('.');
					auto forward_mod   = exp.m_forward_string.substr(0, dot);
					auto forward_symb  = exp.m_forward_string.substr(dot+1);

					std::transform(forward_mod.begin(), forward_mod.end(), forward_mod.begin(), &tolower);
					exp.m_address = resolve(forward_mod, forward_symb);

					if (exp.m_address == 0) {
						if (resolve_api_set(forward_mod) == "kernel32.dll" && entry.first == "kernel32.dll") {
							exp.m_address = resolve("kernelbase.dll", forward_symb);
						}
					}
				}
			}
		}
	}

	uintptr_t import_tracker::resolve(std::string module, std::string name) {
		std::string module_name = resolve_api_set(module);

		auto mod_ittr = m_modules.find(module_name);
		if (mod_ittr != m_modules.end()) {
			auto exp_ittr = mod_ittr->second.m_exports.find(name);
			if (exp_ittr != mod_ittr->second.m_exports.end()) {
				return exp_ittr->second.m_address;
			}
		}

		return 0;
	}

	uintptr_t import_tracker::resolve(std::string module, uint16_t ordinal) {
		std::string module_name = resolve_api_set(module);

		auto mod_ittr = m_modules.find(module_name);
		if (mod_ittr != m_modules.end()) {
			for (auto exp : mod_ittr->second.m_exports) {
				if (exp.second.m_ordinal == ordinal)
					return exp.second.m_address;
			}
		}

		return 0;
	}

	std::map<std::string, import_tracker::remote_export> import_tracker::get_remote_exports(uintptr_t module_base) {
		std::map<std::string, import_tracker::remote_export> exports;

		unsigned char header[0x1000];
		m_target_process->read(module_base, &header, 0x1000);
		auto dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(header);
		auto nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>((uintptr_t)dos_header + dos_header->e_lfanew);

		auto exp_directory = &nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
		if (exp_directory->VirtualAddress == 0)
			return exports;

		IMAGE_EXPORT_DIRECTORY export_dir;
		m_target_process->read(module_base + exp_directory->VirtualAddress, &export_dir, sizeof(IMAGE_EXPORT_DIRECTORY));

		uint16_t *ordinal_table	 = new uint16_t[export_dir.NumberOfNames];
		uint32_t *name_table	 = new uint32_t[export_dir.NumberOfNames];
		uint32_t *function_table = new uint32_t[export_dir.NumberOfFunctions];

		m_target_process->read(module_base + export_dir.AddressOfNameOrdinals, ordinal_table,  sizeof(uint16_t) * export_dir.NumberOfNames);
		m_target_process->read(module_base + export_dir.AddressOfNames,        name_table,     sizeof(uint32_t) * export_dir.NumberOfNames);
		m_target_process->read(module_base + export_dir.AddressOfFunctions,    function_table, sizeof(uint32_t) * export_dir.NumberOfFunctions);

		char temp_name_buffer[255];
		for (unsigned int i = 0; i < export_dir.NumberOfNames; i++) {
			m_target_process->read(module_base + name_table[i], &temp_name_buffer, 255);
			uint32_t    offset = function_table[ordinal_table[i]];

			if (offset != 0) {
				remote_export entry;
				entry.m_name    = temp_name_buffer;
				entry.m_ordinal = ordinal_table[i];
				entry.m_address = module_base + offset;
				entry.m_is_forwarded = false;

				if (offset >= exp_directory->VirtualAddress && offset <= (exp_directory->VirtualAddress + exp_directory->Size)) {
					m_target_process->read(module_base + offset, &temp_name_buffer, 255);
					entry.m_forward_string = temp_name_buffer;
					entry.m_address = 0;
					entry.m_is_forwarded = true;
				}

				exports[entry.m_name] = entry;
			}
		}

		return exports;
	}

	std::string import_tracker::resolve_api_set(std::string api_set_string) {
		std::string fixed_name = api_set_string;
		if (api_set_string.find("api-") == 0 || api_set_string.find("ext-") == 0) {
			std::wstring output;
			std::wstring input = to_wide(api_set_string);

			unsigned char success = false;
			api::api_set_resolve_to_host(input, &success, output);

			if (success) {
				fixed_name = to_narrow(output);
			}
		}

		// forward strings tend to not have .dll on the end, our shit needs it
		if (fixed_name.find(".dll") == std::string::npos) {
			fixed_name += ".dll";
		}

		return fixed_name;
	}
}