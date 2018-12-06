#include "common.h"
#include "misc.h"
#include "api.h"
#include "process.h"

int main(int argc, char **argv) {
	printf("norton gamesafe\n");
	norton::api::resolve_api();

	norton::process proc;
	proc.attach(GetCurrentProcessId());

	for (auto mod : proc.list_modules()) {
		printf("%p ] %s\n", mod.m_base, mod.m_name.c_str());
	}

	getchar();
	return 0;
}