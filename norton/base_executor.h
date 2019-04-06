#pragma once

namespace norton {
	class base_executor {
	public:
		virtual bool call_entry(uintptr_t dll_main, uintptr_t remote_module) abstract;
	};
}