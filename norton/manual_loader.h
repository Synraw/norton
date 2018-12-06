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

		process *m_target_process;
		pe_mapper *m_mapper;
		import_tracker *m_imports;
		base_executor *m_executor;
	};
}