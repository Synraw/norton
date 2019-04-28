#pragma once
#include "common.h"
#include "process.h"

namespace norton {
	bool native_loader_load(process *proc, std::string module_path);
}