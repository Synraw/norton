#pragma once
#include "common.h"

namespace norton {
	struct basic_module {
		std::string m_name;
		uintptr_t   m_base;
	};

	class process {
	public:
		virtual ~process() = default;

		virtual void attach(unsigned long pid);
		virtual void detach();

		virtual uintptr_t alloc(size_t size);
		virtual bool free(uintptr_t addr);
		virtual bool read(uintptr_t addr, void *buffer, size_t size);
		virtual bool write(uintptr_t addr, void *buffer, size_t size);
		virtual void *create_thread(uintptr_t address, void *args);

		virtual uintptr_t get_peb();
		std::vector<basic_module> list_modules();

		unsigned long get_process_id() { return m_pid; }

	private:
		unsigned long m_pid = 0;
		void *m_handle = nullptr;
	};
}