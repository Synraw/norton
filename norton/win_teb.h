#pragma once

#include "win.h"

namespace win {
	typedef struct _TEB {
		PVOID Reserved1[12];
		void  *ProcessEnvironmentBlock;
		PVOID Reserved2[399];
		BYTE Reserved3[1952];
		PVOID TlsSlots[64];
		BYTE Reserved4[8];
		PVOID Reserved5[26];
		PVOID ReservedForOle;  // Windows 2000 only
		PVOID Reserved6[4];
		PVOID TlsExpansionSlots;
	} TEB, *PTEB;
}

