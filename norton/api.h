#pragma once
#include "win.h"

namespace norton {
	namespace api {
		void resolve_api();

		NTSTATUS query_system_information(win::_SYSTEM_INFORMATION_CLASS info_class, void *info, unsigned long length, unsigned long* return_length);
		bool query_information_process(void * handle, win::process_info_class info_class, void *info, unsigned long length);
	}
}