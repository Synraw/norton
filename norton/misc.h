#pragma once

#include "common.h"

namespace norton {
	uintptr_t get_local_teb();
	uintptr_t get_local_peb();

	struct loaded_module {
		std::string m_module_name;
		uintptr_t   m_base;
		uint16_t    m_tls_index;
		unsigned long m_size;
	};

	std::vector<loaded_module> get_loaded_modules();

	bool is_supported_api_schema();

	std::wstring to_wide(const std::string& s);
	std::string to_narrow(const std::wstring& s);

	template<typename T>
	inline T get_absolute_address(void *instruction, uint8_t skip = 0x1, uint8_t size = 0x5) {
		if (!instruction)
			return 0;

		int relative = *(int *)((uint8_t *)(instruction)+skip);
		return (T)(relative + (char *)instruction + size);
	}

	std::string get_working_directory();
	void *load_file_to_memory(std::string name);

	unsigned long get_running_thread_by_pid(unsigned long pid);
	unsigned long get_pid_by_process_name(std::string process_name);
}