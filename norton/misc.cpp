#include "misc.h"

#include "win.h"

namespace norton {
	uintptr_t get_local_teb() {
	#if defined(_M_X64)
		return (uintptr_t)__readgsqword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self));
	#else
		return (uintptr_t)__readfsdword(reinterpret_cast<DWORD_PTR>(&static_cast<NT_TIB*>(nullptr)->Self));
	#endif
	}

	uintptr_t get_local_peb() {
		return (uintptr_t)reinterpret_cast<win::TEB*>(get_local_teb())->ProcessEnvironmentBlock;
	}

	std::vector<loaded_module> get_loaded_modules() {
		std::vector<loaded_module> results;

		char name_conversion_buf[255];
		win::PPEB peb = (win::PPEB)get_local_peb();
		if (peb) {
			auto loaded_mods = &peb->Ldr->InMemoryOrderModuleList;

			for (auto entry = loaded_mods->Flink; entry != loaded_mods; entry = entry->Flink) {
				win::LDR_MODULE* record = CONTAINING_RECORD(entry, win::LDR_MODULE, InMemoryOrderModuleList);
				memset(name_conversion_buf, 0, 255);

				if (wcstombs(name_conversion_buf, record->BaseDllName.Buffer, 255) == 255)
					name_conversion_buf[254] = 0;

				std::string module_name = name_conversion_buf;
				std::transform(module_name.begin(), module_name.end(), module_name.begin(), &tolower);

				loaded_module m;
				m.m_module_name = module_name;
				m.m_base = reinterpret_cast<uintptr_t>(record->BaseAddress);
				m.m_tls_index = (uint16_t)(record->TlsIndex);
				m.m_size = (unsigned long)(record->SizeOfImage);

				results.push_back(m);
			}
		}

		return results;
	}

	bool is_supported_api_schema() {
		win::PPEB peb = (win::PPEB)get_local_peb();
		win::API_SET_NAMESPACE* api_set = (win::API_SET_NAMESPACE *)peb->ApiSetMap;
		return api_set->Version >= 6;
	}

	std::wstring to_wide(const std::string& s) {
		std::wstring temp(s.length(), L' ');
		std::copy(s.begin(), s.end(), temp.begin());
		return temp;
	}

	std::string to_narrow(const std::wstring& s) {
		std::string temp(s.length(), ' ');
		std::copy(s.begin(), s.end(), temp.begin());
		return temp;
	}

	std::string get_working_directory() {
		char buffer[MAX_PATH];
		GetCurrentDirectory(MAX_PATH, buffer);
		return std::string(buffer);
	}

	void *load_file_to_memory(std::string name) {
		auto file = CreateFileA(name.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file == INVALID_HANDLE_VALUE)
			return nullptr;

		auto file_size = GetFileSize(file, NULL);
		void *data = malloc(file_size);

		unsigned long bytes = 0;
		if (!ReadFile(file, data, file_size, &bytes, NULL)) {
			free(data);
			return nullptr;
		}

		CloseHandle(file);
		return data;
	}
}