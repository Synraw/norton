#include <Windows.h>

bool __stdcall DllMain(void *inst, int reason, void *reserved) {
	switch (reason) {
	case DLL_PROCESS_ATTACH:
		Beep(200, 500);
		MessageBoxA(NULL, "a", "B", MB_OK);
		break;
	}

	return true;
}