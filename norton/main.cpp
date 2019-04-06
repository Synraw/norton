#include "common.h"
#include "misc.h"
#include "api.h"
#include "process.h"
#include "import_tracker.h"
#include "manual_loader.h"

int main(int argc, char **argv) {
	printf("norton gamesafe\n");

	if (!norton::is_supported_api_schema()) {
		printf("warn: norton does not support this version of windows\n");
	}

	LoadLibraryA("user32.dll");

	norton::api::resolve_api();

	norton::process proc;
	proc.attach(GetCurrentProcessId());

	norton::import_tracker tracker;
	tracker.setup_process(&proc);

	norton::manual_loader loader;
	if (loader.inject(norton::get_working_directory() + "\\..\\Release\\test_dll_x64.dll", &proc)) {
		printf("successfully injected!\n");
	}
	else {
		printf("failed to inject!\n");
	}

	getchar();
	return 0;
}