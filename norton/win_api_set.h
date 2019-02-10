#pragma once

#include "win.h"

namespace win {
	typedef struct {
		ULONG Version;     // v2 on Windows 7, v4 on Windows 8.1  and v6 on Windows 10
		ULONG Size;        // apiset map size (usually the .apiset section virtual size)
		ULONG Flags;       // according to Geoff Chappell,  tells if the map is sealed or not.
		ULONG Count;       // hash table entry count
		ULONG EntryOffset; // Offset to the api set entries values
		ULONG HashOffset;  // Offset to the api set entries hash indexes
		ULONG HashFactor;  // multiplier to use when computing hash 
	} API_SET_NAMESPACE;


}