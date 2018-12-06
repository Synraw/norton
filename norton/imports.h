#pragma once

#include "common.h"

namespace norton {
	struct module_cache {
		uintptr_t	m_base;
		std::map<std::string, uintptr_t> m_exports;
	};

	class import_resolver {
	public:
		void update_cache();
		uintptr_t resolve(std::string module, std::string export_name);
	private:
		std::map<std::string, module_cache> m_modules;
	};
}