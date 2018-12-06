#pragma once

#include "win.h"

namespace win {
	typedef enum _PROCESSINFOCLASS {
		ProcessBasicInformation,
		ProcessQuotaLimits,
		ProcessIoCounters,
		ProcessVmCounters,
		ProcessTimes,
		ProcessBasePriority,
		ProcessRaisePriority,
		ProcessDebugPort,
		ProcessExceptionPort,
		ProcessAccessToken,
		ProcessLdtInformation,
		ProcessLdtSize,
		ProcessDefaultHardErrorMode,
		ProcessIoPortHandlers,          // Note: this is kernel mode only
		ProcessPooledUsageAndLimits,
		ProcessWorkingSetWatch,
		ProcessUserModeIOPL,
		ProcessEnableAlignmentFaultFixup,
		ProcessPriorityClass,
		ProcessWx86Information,
		ProcessHandleCount,
		ProcessAffinityMask,
		ProcessPriorityBoost,
		ProcessDeviceMap,
		ProcessSessionInformation,
		ProcessForegroundInformation,
		ProcessWow64Information,
		ProcessImageFileName,
		ProcessLUIDDeviceMapsEnabled,
		ProcessBreakOnTermination,
		ProcessDebugObjectHandle,
		ProcessDebugFlags,
		ProcessHandleTracing,
		ProcessIoPriority,
		ProcessExecuteFlags,
		ProcessTlsInformation,
		ProcessCookie,
		ProcessImageInformation,
		ProcessCycleTime,
		ProcessPagePriority,
		ProcessInstrumentationCallback,
		ProcessThreadStackAllocation,
		ProcessWorkingSetWatchEx,
		ProcessImageFileNameWin32,
		ProcessImageFileMapping,
		ProcessAffinityUpdateMode,
		ProcessMemoryAllocationMode,
		ProcessGroupInformation,
		ProcessTokenVirtualizationEnabled,
		ProcessConsoleHostProcess,
		ProcessWindowInformation,
		MaxProcessInfoClass             // MaxProcessInfoClass should always be the last enum
	} process_info_class;

	typedef struct _PROCESS_BASIC_INFORMATION {
		NTSTATUS ExitStatus;
		PPEB PebBaseAddress;
		ULONG_PTR AffinityMask;
		LONG BasePriority;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR InheritedFromUniqueProcessId;
	} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;
}