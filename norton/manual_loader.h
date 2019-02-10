#pragma once

#include "common.h"
#include "process.h"
#include "pe.h"

#include "pe_mapper.h"
#include "import_tracker.h"
#include "base_executor.h"

namespace norton {
	struct manual_loader_context {
		std::string m_file_name;
		void *m_module_buffer;
		pe *m_pe;

		process        *m_process;
		pe_mapper      *m_mapper;
		import_tracker *m_imports;
		base_executor  *m_executor;
	};

	class manual_loader {
	public:
		bool inject(manual_loader_context& context);
		bool inject(std::string file, process proc);
		bool inject(void *buffer, process proc);
	private:

		bool run_current_context();
		manual_loader_context m_context;
	};
}