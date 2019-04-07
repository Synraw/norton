#pragma once
#include "process.h"
#include "base_executor.h"

namespace norton {
	class hijack_executor : public base_executor {
	public:
		hijack_executor(process *proc);
		virtual bool call_entry(uintptr_t dll_main, uintptr_t remote_module) override;
	private:
		process *m_process;
	};
}