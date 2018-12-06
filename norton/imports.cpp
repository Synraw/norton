#include "imports.h"

#include "win.h"
#include "misc.h"
#include "pe.h"

namespace norton {
	void import_resolver::update_cache() {
		static auto process_module = [this](loaded_module m) -> void {
			module_cache mod;
			mod.m_base = m.m_base;

			pe p((void *)m.m_base, true);
			if (p.is_valid()) {
				for (auto exp : p.get_exports()) {
					mod.m_exports[exp.m_name] = m.m_base + exp.m_offset;
				}
			}

			m_modules[m.m_module_name] = mod;
		};

		auto modules = get_loaded_modules();
		for (auto module : modules)
			process_module(module);
	}

	uintptr_t import_resolver::resolve(std::string module, std::string export_name) {
		auto mod_ittr = m_modules.find(module);
		if (mod_ittr != m_modules.end()) {
			auto exp_ittr = mod_ittr->second.m_exports.find(export_name);
			if (exp_ittr != mod_ittr->second.m_exports.end())
				return exp_ittr->second;
		}

		return 0;
	}
}