#pragma once
#include "win.h"

namespace win {
	typedef LONG 	KPRIORITY;

	typedef enum _KWAIT_REASON {
		Executive,
		FreePage,
		PageIn,
		PoolAllocation,
		DelayExecution,
		Suspended,
		UserRequest,
		WrExecutive,
		WrFreePage,
		WrPageIn,
		WrPoolAllocation,
		WrDelayExecution,
		WrSuspended,
		WrUserRequest,
		WrEventPair,
		WrQueue,
		WrLpcReceive,
		WrLpcReply,
		WrVirtualMemory,
		WrPageOut,
		WrRendezvous,
		WrKeyedEvent,
		WrTerminated,
		WrProcessInSwap,
		WrCpuRateControl,
		WrCalloutStack,
		WrKernel,
		WrResource,
		WrPushLock,
		WrMutex,
		WrQuantumEnd,
		WrDispatchInt,
		WrPreempted,
		WrYieldExecution,
		WrFastMutex,
		WrGuardedMutex,
		WrRundown,
		WrAlertByThreadId,
		WrDeferredPreempt,
		MaximumWaitReason
	} KWAIT_REASON, *PKWAIT_REASON;

	typedef struct _SYSTEM_THREAD_INFORMATION
	{
		LARGE_INTEGER KernelTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER CreateTime;
		ULONG WaitTime;
		PVOID StartAddress;
		CLIENT_ID ClientId;
		KPRIORITY Priority;
		LONG BasePriority;
		ULONG ContextSwitches;
		ULONG ThreadState;
		KWAIT_REASON WaitReason;
	} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

	typedef struct _VM_COUNTERS {
		SIZE_T		   PeakVirtualSize;	// not actually
		SIZE_T         PageFaultCount;
		SIZE_T         PeakWorkingSetSize;
		SIZE_T         WorkingSetSize;
		SIZE_T         QuotaPeakPagedPoolUsage;
		SIZE_T         QuotaPagedPoolUsage;
		SIZE_T         QuotaPeakNonPagedPoolUsage;
		SIZE_T         QuotaNonPagedPoolUsage;
		SIZE_T         PagefileUsage;
		SIZE_T         PeakPagefileUsage;
		SIZE_T         VirtualSize;		// not actually
	} VM_COUNTERS;

	typedef struct _SYSTEM_PROCESS_INFORMATION
	{
		ULONG NextEntryOffset;
		ULONG NumberOfThreads;
		LARGE_INTEGER WorkingSetPrivateSize; // since VISTA
		ULONG HardFaultCount; // since WIN7
		ULONG NumberOfThreadsHighWatermark; // since WIN7
		ULONGLONG CycleTime; // since WIN7
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		KPRIORITY BasePriority;
		HANDLE UniqueProcessId;
		HANDLE InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		ULONG_PTR UniqueProcessKey; // since VISTA (requires SystemExtendedProcessInformation)
		VM_COUNTERS VmCounter;
		SIZE_T PrivatePageCount;
		IO_COUNTERS IoCount;
		SYSTEM_THREAD_INFORMATION Threads[1];
	} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;
}