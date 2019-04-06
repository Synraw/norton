#pragma once
#include "process.h"
#include "base_executor.h"

namespace norton {
	class thread_executor : public base_executor {
	public:
		thread_executor(process *proc);
		virtual bool call_entry(uintptr_t dll_main, uintptr_t remote_module) override;
	private:
		process *m_process;
	};
}