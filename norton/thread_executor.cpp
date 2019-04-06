#include "thread_executor.h"

namespace norton {
	thread_executor::thread_executor(process *proc) : m_process{ proc } {

	}

	bool thread_executor::call_entry(uintptr_t dll_main, uintptr_t remote_module) {
		std::vector<unsigned char> stub;
		stub = { 0x48, 0x83, 0xEC, 0x28, 0x48, 0xB9, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x48, 0xC7, 0xC2, 0x01, 0x00, 0x00, 0x00, 0x4D, 0x31, 0xC0, 0x48, 0xB8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xFF, 0xD0, 0x48, 0x83, 0xC4, 0x28, 0xC3 };
		*(uintptr_t *)&stub[6]  = remote_module;
		*(uintptr_t *)&stub[26] = dll_main;

		uintptr_t exec_stub = m_process->alloc(stub.size());
		m_process->write(exec_stub, stub.data(), stub.size());

		printf("exec_stub: %p\n", exec_stub);
		getchar();
		m_process->create_thread(exec_stub, 0);
	}
}