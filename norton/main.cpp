#include "common.h"
#include "misc.h"
#include "api.h"
#include "process.h"
#include "import_tracker.h"

int main(int argc, char **argv) {
	printf("norton gamesafe\n");

	if (!norton::is_supported_api_schema()) {
		printf("warn: norton does not support this version of windows\n");
	}

	norton::api::resolve_api();

	norton::process proc;
	proc.attach(GetCurrentProcessId());

	norton::import_tracker tracker;
	tracker.setup_process(&proc);

	getchar();
	return 0;
}