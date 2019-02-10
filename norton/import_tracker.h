#pragma once
#include "common.h"
#include "process.h"

namespace norton {
	class import_tracker {
	public:
		void setup_process(process *proc);

		uintptr_t resolve(std::string module, std::string name);
		uintptr_t resolve(std::string module, uint16_t ordinal);

	private:
		struct remote_export {
			std::string m_name;
			uint16_t m_ordinal;
			uintptr_t m_address;
			bool m_is_forwarded;
			std::string m_forward_string;
		};

		struct remote_module {
			std::string m_name;
			uintptr_t   m_base;
			std::map<std::string, remote_export> m_exports;
		};

		process *m_target_process;
		std::map<std::string, remote_module> m_modules;

		std::map<std::string, remote_export> get_remote_exports(uintptr_t module_base);
		std::string resolve_api_set(std::string api_set_string);
	};
}