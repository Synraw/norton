#pragma once

#include "win.h"

namespace win {
	struct CLIENT_ID
	{
		UINT64 UniqueProcess;
		UINT64 UniqueThread;
	};
}