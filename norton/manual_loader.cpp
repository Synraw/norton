#include "manual_loader.h"
#include "thread_executor.h"
#include "misc.h"

namespace norton {
	bool manual_loader::inject(manual_loader_context& context) {

	}

	bool manual_loader::inject(std::string file, process *proc) {
		m_context.m_file_name = file;
		m_context.m_process   = proc;
		
		m_context.m_module_buffer = norton::load_file_to_memory(m_context.m_file_name);
		if (!m_context.m_module_buffer) {
			printf("failed to load file to memory: %s\n", file.c_str());
			return false;
		}

		m_context.m_pe			  = new pe(m_context.m_module_buffer);
		if (!m_context.m_pe->is_valid()) {
			printf("failed to parse pe file\n");
			return false;
		}

		m_context.m_imports       = new import_tracker();
		m_context.m_imports->setup_process(m_context.m_process);

		m_context.m_remote_buffer = 0;
		m_context.m_mapper        = new pe_mapper();
		m_context.m_executor	  = new thread_executor(proc);

		return run_current_context();
	}

	bool manual_loader::inject(void *buffer, process *proc) {

	}

	bool manual_loader::run_current_context() {
		bool mapped = m_context.m_mapper->map_image(m_context);
		if (!mapped) {
			return false;
		}

		uintptr_t dll_main = m_context.m_pe->get_nt()->OptionalHeader.AddressOfEntryPoint + m_context.m_remote_buffer;
		return m_context.m_executor->call_entry(dll_main, m_context.m_remote_buffer);
	}
}