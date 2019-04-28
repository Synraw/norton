#include "ldr_load.h"

namespace norton {
	bool native_loader_load(process *proc, std::string module_path) {
		auto remote_path = proc->alloc(module_path.size());
		proc->write(remote_path, (void *)module_path.c_str(), module_path.size());
		proc->create_thread((uintptr_t)&LoadLibraryA, (void *)remote_path);

		return true;
	}
}