#include "pe.h"
#include "win.h"

namespace norton {
	inline bool section_contains(IMAGE_SECTION_HEADER* section, uintptr_t address) {
		return (address >= section->VirtualAddress && address < (section->VirtualAddress + (section->Misc.VirtualSize == 0 ? section->SizeOfRawData : section->Misc.VirtualSize)));
	}

	uintptr_t pe::rva_to_ptr(uintptr_t rva) {
		if (m_loaded)
			return (uintptr_t)m_bytes + rva;

		for (auto sec : m_sections) {
			if (section_contains(sec, rva)) {
				uintptr_t delta = (uintptr_t)(sec->VirtualAddress - sec->PointerToRawData);
				return (uintptr_t)((uintptr_t)m_bytes + rva - delta);
			}
		}

		return 0;
	}

	PIMAGE_DATA_DIRECTORY pe::get_data_directory(unsigned int index) {
		if (index < IMAGE_NUMBEROF_DIRECTORY_ENTRIES)
			return &m_nt_header->OptionalHeader.DataDirectory[index];
		return nullptr;
	}

	bool pe::process_image() {
		if (!m_bytes) return false;

		m_dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(m_bytes);
		if (m_dos_header->e_magic != IMAGE_DOS_SIGNATURE)
			return false;

		m_nt_header = reinterpret_cast<PIMAGE_NT_HEADERS>((uintptr_t)m_dos_header + m_dos_header->e_lfanew);
		if (m_nt_header->Signature != IMAGE_NT_SIGNATURE)
			return false;

		m_x64 = m_nt_header->FileHeader.Machine == IMAGE_FILE_MACHINE_AMD64;

		process_sections();
		process_imports();
		process_exports();

		if (m_loaded == false) {
			process_relocs();

		}

		return true;
	}

	void pe::process_sections() {
		if (m_sections.empty()) {
			size_t count = m_nt_header->FileHeader.NumberOfSections;
			m_sections.reserve(count);

			unsigned int i = 0;
			for (PIMAGE_SECTION_HEADER sec_ittr = IMAGE_FIRST_SECTION(m_nt_header); i < count; sec_ittr++, i++) {
				m_sections.push_back(sec_ittr);
			}
		}
	}

	void pe::process_imports() {
		if (m_imports.empty()) {
			auto imp_directory = get_data_directory(IMAGE_DIRECTORY_ENTRY_IMPORT);
			if (!imp_directory->VirtualAddress || !imp_directory->Size)
				return;

			PIMAGE_IMPORT_DESCRIPTOR import_desc = (PIMAGE_IMPORT_DESCRIPTOR)rva_to_ptr(imp_directory->VirtualAddress);

			if (!import_desc)
				return;

			for (; import_desc->OriginalFirstThunk && import_desc->Name; import_desc++) {
				int iat_index = 0;

				for (PIMAGE_THUNK_DATA current_thunk = (PIMAGE_THUNK_DATA)rva_to_ptr(import_desc->OriginalFirstThunk); current_thunk->u1.AddressOfData > 0; current_thunk++) {

					pe_import imp;
					imp.m_module = (const char *)rva_to_ptr(import_desc->Name);
					imp.m_func_ptr = (uintptr_t *)rva_to_ptr(import_desc->FirstThunk + iat_index);

					// lowercase module names for consistency
					std::transform(imp.m_module.begin(), imp.m_module.end(), imp.m_module.begin(), &tolower);

					// add to module list if we havent already got it
					if (std::find(m_import_modules.begin(), m_import_modules.end(), imp.m_module) == m_import_modules.end()) {
						m_import_modules.push_back(imp.m_module);
					}

					if (current_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) {
						imp.m_is_ordinal = true;
						imp.m_ordinal = (unsigned short)(current_thunk->u1.Ordinal & ~(IMAGE_ORDINAL_FLAG));
						imp.m_func = std::to_string(imp.m_ordinal);
					}
					else {
						imp.m_is_ordinal = false;
						imp.m_ordinal = 0;
						imp.m_func = ((PIMAGE_IMPORT_BY_NAME)(rva_to_ptr(current_thunk->u1.AddressOfData)))->Name;
					}

					m_imports.push_back(imp);
					iat_index += m_x64 ? 8 : 4;
				}
			}
		}
	}

	void pe::process_relocs() {
		if (m_relocs.empty()) {
			auto reloc_dir_desc = get_data_directory(IMAGE_DIRECTORY_ENTRY_BASERELOC);
			if (!reloc_dir_desc->VirtualAddress || !reloc_dir_desc->Size)
				return;

			PIMAGE_BASE_RELOCATION reloc_dir = (PIMAGE_BASE_RELOCATION)rva_to_ptr(reloc_dir_desc->VirtualAddress);

			for (; reloc_dir->VirtualAddress > 0; reloc_dir = (PIMAGE_BASE_RELOCATION)((uintptr_t)reloc_dir + reloc_dir->SizeOfBlock)) {
				int count = (reloc_dir->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);
				uint16_t *reloc_list = (uint16_t *)(reloc_dir + 1);

				for (int i = 0; i < count; i++) {
					if (reloc_list[i]) {
						pe_reloc r;
						r.m_flags = reloc_list[i] >> 12;
						r.m_reference_addr = (void *)rva_to_ptr(reloc_dir->VirtualAddress + (reloc_list[i] & 0xfff));

						if (r.m_flags == IMAGE_REL_BASED_HIGHADJ) {
							if (((reloc_list[i] & 0xfff) & LDRP_RELOCATION_FINAL) == 0) {
								// high adj has an extra second slot
								i++;
								r.m_highadj_extra = (*(short *)&reloc_list[i]);
							}
							else {
								continue;
							}
						}

						m_relocs.push_back(r);
					}
				}
			}
		}
	}

	void pe::process_exports() {
		if (m_exports.empty()) {
			auto exp_directory = get_data_directory(IMAGE_DIRECTORY_ENTRY_EXPORT);
			if (!exp_directory->VirtualAddress)
				return;

			PIMAGE_EXPORT_DIRECTORY export_dir = (PIMAGE_EXPORT_DIRECTORY)rva_to_ptr(exp_directory->VirtualAddress);

			if (export_dir) {
				m_exports.reserve(export_dir->NumberOfNames);

				uint16_t *ordinal_table = (uint16_t *)(rva_to_ptr(export_dir->AddressOfNameOrdinals));
				uint32_t *name_table = (uint32_t *)(rva_to_ptr(export_dir->AddressOfNames));
				uint32_t *function_table = (uint32_t *)(rva_to_ptr(export_dir->AddressOfFunctions));

				for (unsigned int i = 0; i < export_dir->NumberOfNames; i++) {
					std::string name = (const char *)rva_to_ptr(name_table[i]);
					uint32_t    offs = function_table[ordinal_table[i]];

					if (offs == 0) continue;

					pe_export exp;
					exp.m_offset = offs;
					exp.m_name = name;

					// forwarded export handling
					if (offs >= exp_directory->VirtualAddress && offs <= (exp_directory->VirtualAddress + exp_directory->Size)) {
						exp.m_forward_string = (const char *)rva_to_ptr(offs);
						exp.m_offset = 0;
					}

					m_exports.push_back(exp);
				}
			}
		}
	}

	uintptr_t pe::get_unpacked_iat_offset(std::string mod, std::string name) {
		auto imp_directory = get_data_directory(IMAGE_DIRECTORY_ENTRY_IMPORT);
		if (!imp_directory->VirtualAddress || !imp_directory->Size)
			return 0;

		PIMAGE_IMPORT_DESCRIPTOR import_desc = (PIMAGE_IMPORT_DESCRIPTOR)rva_to_ptr(imp_directory->VirtualAddress);

		if (!import_desc)
			return 0;

		for (; import_desc->OriginalFirstThunk && import_desc->Name; import_desc++) {
			int iat_index = 0;

			for (PIMAGE_THUNK_DATA current_thunk = (PIMAGE_THUNK_DATA)rva_to_ptr(import_desc->OriginalFirstThunk); current_thunk->u1.AddressOfData > 0; current_thunk++) {

				if ((current_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0) {
					std::string mod_name = (const char *)rva_to_ptr(import_desc->Name);
					std::transform(mod_name.begin(), mod_name.end(), mod_name.begin(), &tolower);
					std::string func_name = ((PIMAGE_IMPORT_BY_NAME)(rva_to_ptr(current_thunk->u1.AddressOfData)))->Name;

					if (mod == mod_name && name == func_name) {
						return import_desc->FirstThunk + iat_index;
					}
				}

				iat_index += m_x64 ? 8 : 4;
			}
		}

		return 0;
	}
}