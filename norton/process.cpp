#include "process.h"
#include "win.h"
#include "api.h"

namespace norton {
	void process::attach(unsigned long pid) {
		m_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	}

	void process::detach() {
		CloseHandle(m_handle);
	}

	uintptr_t process::alloc(size_t size) {
		return reinterpret_cast<uintptr_t>(VirtualAllocEx(m_handle, nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));
	}

	bool process::free(uintptr_t addr) {
		return VirtualFreeEx(m_handle, (void *)addr, 0, MEM_RELEASE);
	}

	bool process::read(uintptr_t addr, void *buffer, size_t size) {
		return ReadProcessMemory(m_handle, (void *)addr, buffer, size, nullptr);
	}

	bool process::write(uintptr_t addr, void *buffer, size_t size) {
		return WriteProcessMemory(m_handle, (void *)addr, buffer, size, nullptr);
	}

	void *process::create_thread(uintptr_t address, void *args) {
		return CreateRemoteThread(m_handle, 0, 0, (LPTHREAD_START_ROUTINE)address, args, 0, 0);
	}

	uintptr_t process::get_peb() {
		win::PROCESS_BASIC_INFORMATION info = { 0 };
		if (!api::query_information_process(m_handle, win::ProcessBasicInformation, &info, sizeof(win::PROCESS_BASIC_INFORMATION)))
			return 0;

		return (uintptr_t)info.PebBaseAddress;
	}

	std::vector<basic_module> process::list_modules() {
		auto peb_addr = get_peb();
		std::vector<basic_module> result;

		win::PEB peb;
		read(peb_addr, &peb, sizeof(win::PEB));

		uintptr_t head_addr = (uintptr_t)(peb.Ldr) + offsetof(win::PEB_LDR_DATA, InMemoryOrderModuleList);
		LIST_ENTRY head_entry, *entry_ptr, entry;

		read(head_addr, &head_entry, sizeof(LIST_ENTRY));
		entry_ptr = head_entry.Flink;

		win::LDR_MODULE ldr_entry;
		read((uintptr_t)entry_ptr, &entry, sizeof(LIST_ENTRY));

		while ((uintptr_t)entry_ptr != head_addr) {
			read((uintptr_t)CONTAINING_RECORD(entry_ptr, win::LDR_MODULE, InMemoryOrderModuleList), &ldr_entry, sizeof(win::LDR_MODULE));
			WCHAR buffer[255];
			char abuffer[255];

			read((uintptr_t)ldr_entry.BaseDllName.Buffer, buffer, sizeof(WCHAR)*(ldr_entry.BaseDllName.Length + 1));
			if (wcstombs(abuffer, buffer, 255) == 255)
				abuffer[254] = 0;

			basic_module m;
			m.m_name = abuffer;
			std::transform(m.m_name.begin(), m.m_name.end(), m.m_name.begin(), &tolower);
			m.m_base = (uintptr_t)ldr_entry.BaseAddress;
			result.push_back(m);

			entry_ptr = entry.Flink;
			read((uintptr_t)entry_ptr, &entry, sizeof(LIST_ENTRY));
		}

		return result;
	}
}