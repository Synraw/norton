#include "api.h"

#include "imports.h"

namespace norton {
	import_resolver g_imports;

	namespace api {
		void resolve_api() {
			g_imports.update_cache();
		}

		NTSTATUS query_system_information(win::_SYSTEM_INFORMATION_CLASS info_class, void *info, unsigned long length, unsigned long* return_length) {
			using _qsi = NTSTATUS(WINAPI *)(win::_SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
			auto fn = (_qsi)g_imports.resolve("ntdll.dll", "NtQuerySystemInformation");

			return fn(info_class, info, length, return_length);
		}

		bool query_information_process(void *handle, win::process_info_class info_class, void *info, unsigned long length) {
			using _qip = NTSTATUS(NTAPI *)(HANDLE, win::process_info_class, PVOID, ULONG, PULONG);
			auto fn = (_qip)g_imports.resolve("ntdll.dll", "NtQueryInformationProcess");
			return NT_SUCCESS(fn(handle, info_class, info, length, NULL));
		}
	}
}