#pragma once

#include "common.h"

namespace norton {
	uintptr_t get_local_teb();
	uintptr_t get_local_peb();

	struct loaded_module {
		std::string m_module_name;
		uintptr_t   m_base;
		uint16_t    m_tls_index;
		unsigned long m_size;
	};

	std::vector<loaded_module> get_loaded_modules();
}