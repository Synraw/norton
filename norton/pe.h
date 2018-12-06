#pragma once

#include "common.h"

namespace norton {
	struct pe_import {
		bool m_is_ordinal;
		unsigned short m_ordinal;
		std::string m_module;
		std::string m_func;
		uintptr_t *m_func_ptr;
	};

	struct pe_reloc {
		void *m_reference_addr;
		unsigned char m_flags;
		uintptr_t m_highadj_extra;
	};

	struct pe_export {
		std::string m_name;
		std::string m_forward_string;
		uintptr_t m_offset; // 0 when forwarded
	};

	class pe {
	public:
		pe() {}

		pe(void *data, bool loaded = false) : m_bytes{ data }, m_loaded{ loaded }, m_valid{ false } {
			m_valid = process_image();
		}

		inline size_t header_size() { return m_nt_header->OptionalHeader.SizeOfHeaders; }
		inline size_t module_size() { return m_nt_header->OptionalHeader.SizeOfImage; }
		inline uintptr_t image_base() { return m_nt_header->OptionalHeader.ImageBase; }
		inline uintptr_t entry_point() { return m_nt_header->OptionalHeader.AddressOfEntryPoint; }

		inline bool is_dll() { return (m_nt_header->FileHeader.Characteristics & IMAGE_FILE_DLL) != 0; }
		inline bool is_x64() { return m_x64; }
		inline bool is_valid() { return m_valid; }

		inline PIMAGE_NT_HEADERS get_nt() { return m_nt_header; }
		inline PIMAGE_DOS_HEADER get_dos() { return m_dos_header; }

		uintptr_t rva_to_ptr(uintptr_t rva);
		uintptr_t get_unpacked_iat_offset(std::string mod, std::string name);

		PIMAGE_DATA_DIRECTORY get_data_directory(unsigned int index);
		std::vector<pe_import>& get_imports() { return m_imports; }
		std::vector<std::string>& get_imported_modules() { return m_import_modules; }
		std::vector<pe_export>& get_exports() { return m_exports; }
		std::vector<pe_reloc>& get_relocs() { return m_relocs; }
		std::vector<IMAGE_SECTION_HEADER *>& get_sections() { return m_sections; }
	private:
		void *m_bytes;

		bool m_loaded;
		bool m_valid;
		bool m_x64;

		PIMAGE_DOS_HEADER m_dos_header;
		PIMAGE_NT_HEADERS m_nt_header;

		std::vector<IMAGE_SECTION_HEADER *> m_sections;
		std::vector<pe_import> m_imports;
		std::vector<std::string> m_import_modules;
		std::vector<pe_export> m_exports;
		std::vector<pe_reloc>  m_relocs;

		bool process_image();
		void process_sections();
		void process_imports();
		void process_relocs();
		void process_exports();
	};
}