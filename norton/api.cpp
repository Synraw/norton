#include "api.h"

#include "imports.h"
#include "misc.h"

#if defined(_M_X64)
	#define API_SET_OFFSET 0x4e
#else
	#define API_SET_OFFSET 0x30
#endif

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

		void rtl_init_unicode_string(PUNICODE_STRING unicode_str, PCWSTR data) {
			unicode_str->Length = 0;
			unicode_str->Buffer = (PWSTR)data;

			if (data) {
				int i = -1;
				do ++i;	while (data[i]);
				unicode_str->Length = sizeof(wchar_t) * i;
				unicode_str->MaximumLength = unicode_str->Length + sizeof(wchar_t);
			}
		}

		bool api_set_resolve_to_host(std::wstring api_to_resolve, PBOOLEAN resolved, std::wstring& output) {
			UNICODE_STRING in, out;
			rtl_init_unicode_string(&in, api_to_resolve.c_str());

			using _rth = NTSTATUS(__fastcall *)(win::API_SET_NAMESPACE *, PUNICODE_STRING, PUNICODE_STRING, PBOOLEAN, PUNICODE_STRING);
			auto query_func = g_imports.resolve("ntdll.dll", "ApiSetQueryApiSetPresence") + API_SET_OFFSET;
			auto fn = norton::get_absolute_address<_rth>((void *)query_func);

			win::PPEB peb = (win::PPEB)get_local_peb();
			win::API_SET_NAMESPACE* api_set = (win::API_SET_NAMESPACE *)peb->ApiSetMap;

			auto status = fn(api_set, &in, nullptr, resolved, &out);
			output = std::wstring(out.Buffer, out.Length / sizeof(wchar_t));

			return NT_SUCCESS(status);
		}
	}
}