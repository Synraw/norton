#include "common.h"
#include "misc.h"
#include "api.h"
#include "process.h"
#include "import_tracker.h"
#include "manual_loader.h"

unsigned long hijack_me(void *arg) {
	while (true) {
		
	}

	return 0;
}

int main(int argc, char **argv) {
	printf("norton gamesafe\n");

	if (!norton::is_supported_api_schema()) {
		printf("warn: norton does not support this version of windows\n");
	}

	LoadLibraryA("user32.dll");
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)hijack_me, 0, 0, 0);

	norton::api::resolve_api();

	norton::process proc;
	proc.attach(norton::get_pid_by_process_name("EscapeFromTarkov.exe"));

	norton::manual_loader loader;
	if (loader.inject(norton::get_working_directory() + "\\test_dll_x64.dll", &proc)) {
		printf("successfully injected!\n");
	}
	else {
		printf("failed to inject!\n");
	}

	getchar();
	return 0;
}