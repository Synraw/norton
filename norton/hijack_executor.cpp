#include "hijack_executor.h"
#include "api.h"
#include "misc.h"

namespace norton {
	hijack_executor::hijack_executor(process *proc) : m_process{ proc } {

	}

	bool hijack_executor::call_entry(uintptr_t dll_main, uintptr_t remote_module) {
		std::vector<unsigned char> stub;

		auto tid = get_running_thread_by_pid(m_process->get_process_id());
		if (tid == 0) {
			return false;
		}

		auto thread_handle = OpenThread(THREAD_ALL_ACCESS, false, tid);
		if (thread_handle == INVALID_HANDLE_VALUE)
			return false;

		SuspendThread(thread_handle);

		CONTEXT context;
		context.ContextFlags = CONTEXT_FULL;
		GetThreadContext(thread_handle, &context);

#if defined(_M_X64)
		stub = { 0x48, 0x83, 0xEC, 0x28, 0x48, 0xB9, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x4D, 0x31, 0xC0, 0x48, 0xB8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xFF, 0xD0, 0x48, 0x83, 0xC4, 0x28, 0xC3 };
		*(uintptr_t *)&stub[6] = remote_module;
		*(uintptr_t *)&stub[26] = dll_main;
#else 
		stub = { 0x6A, 0x00, 0x6A, 0x01, 0x68, 0xDD, 0xCC, 0xBB, 0xAA, 0xB9, 0xEE, 0xFF, 0xEE, 0xFF, 0xFF, 0xD1, 0xB8, 0x44, 0x33, 0x22, 0x11, 0x50, 0xC3 };
		*(uintptr_t *)&stub[5] = remote_module;
		*(uintptr_t *)&stub[10] = dll_main;
		*(uintptr_t *)&stub[17] = (uintptr_t)context.Eip;
#endif

		uintptr_t exec_stub = m_process->alloc(stub.size());
		m_process->write(exec_stub, stub.data(), stub.size());

#ifdef _M_X64
		context.Rip = exec_stub;
#else
		context.Eip = exec_stub;
#endif

		SetThreadContext(thread_handle, &context);
		ResumeThread(thread_handle);
		CloseHandle(thread_handle);
		return true;
	}
}