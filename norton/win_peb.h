#pragma once

#include "win.h"

#define CV_SIGNATURE_NB10   '01BN'
#define CV_SIGNATURE_RSDS   'SDSR'

namespace win {
	typedef struct _LDR_MODULE
	{
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID BaseAddress;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG Flags;
		SHORT LoadCount;
		SHORT TlsIndex;
		LIST_ENTRY HashTableEntry;
		ULONG TimeDateStamp;
	} LDR_MODULE, *PLDR_MODULE;

	typedef struct _RTL_USER_PROCESS_PARAMETERS *PRTL_USER_PROCESS_PARAMETERS;
	typedef struct _RTL_CRITICAL_SECTION *PRTL_CRITICAL_SECTION;

	typedef struct _PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		HANDLE SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID EntryInProgress;
		BOOLEAN ShutdownInProgress;
		HANDLE ShutdownThreadId;
	} PEB_LDR_DATA, *PPEB_LDR_DATA;

	typedef struct _PEB
	{
		BOOLEAN InheritedAddressSpace;
		BOOLEAN ReadImageFileExecOptions;
		BOOLEAN BeingDebugged;

		union
		{
			BOOLEAN BitField;

			struct
			{
				BOOLEAN ImageUsesLargePages : 1;
				BOOLEAN IsProtectedProcess : 1;
				BOOLEAN IsImageDynamicallyRelocated : 1;
				BOOLEAN SkipPatchingUser32Forwarders : 1;
				BOOLEAN IsPackagedProcess : 1;
				BOOLEAN IsAppContainer : 1;
				BOOLEAN IsProtectedProcessLight : 1;
				BOOLEAN SpareBits : 1;
			};
		};

		HANDLE Mutant;

		PVOID ImageBaseAddress;
		PPEB_LDR_DATA Ldr;
		PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
		PVOID SubSystemData;
		PVOID ProcessHeap;
		PRTL_CRITICAL_SECTION FastPebLock;
		PVOID AtlThunkSListPtr;
		PVOID IFEOKey;

		union
		{
			ULONG CrossProcessFlags;

			struct
			{
				ULONG ProcessInJob : 1;
				ULONG ProcessInitializing : 1;
				ULONG ProcessUsingVEH : 1;
				ULONG ProcessUsingVCH : 1;
				ULONG ProcessUsingFTH : 1;
				ULONG ReservedBits0 : 27;
			};

			ULONG EnvironmentUpdateCount;
		};

		union
		{
			PVOID KernelCallbackTable;
			PVOID UserSharedInfoPtr;
		};

		ULONG SystemReserved[1];
		ULONG AtlThunkSListPtr32;
		PVOID ApiSetMap;
		ULONG TlsExpansionCounter;
		PVOID TlsBitmap;
		ULONG TlsBitmapBits[2];
		PVOID ReadOnlySharedMemoryBase;
		PVOID HotpatchInformation;
		PVOID ReadOnlyStaticServerData;
		PVOID AnsiCodePageData;
		PVOID OemCodePageData;
		PVOID UnicodeCaseTableData;

		ULONG NumberOfProcessors;
		ULONG NtGlobalFlag;

		LARGE_INTEGER CriticalSectionTimeout;
		SIZE_T HeapSegmentReserve;
		SIZE_T HeapSegmentCommit;
		SIZE_T HeapDeCommitTotalFreeThreshold;
		SIZE_T HeapDeCommitFreeBlockThreshold;

		ULONG NumberOfHeaps;
		ULONG MaximumNumberOfHeaps;
		PVOID ProcessHeaps;

		PVOID GdiSharedHandleTable;
		PVOID ProcessStarterHelper;
		ULONG GdiDCAttributeList;

		PRTL_CRITICAL_SECTION LoaderLock;

		ULONG OSMajorVersion;
		ULONG OSMinorVersion;
		USHORT OSBuildNumber;
		USHORT OSCSDVersion;
		ULONG OSPlatformId;
		ULONG ImageSubsystem;
		ULONG ImageSubsystemMajorVersion;
		ULONG ImageSubsystemMinorVersion;
		ULONG_PTR ImageProcessAffinityMask;
		ULONG GdiHandleBuffer;
		PVOID PostProcessInitRoutine;

		PVOID TlsExpansionBitmap;
		ULONG TlsExpansionBitmapBits[32];

		ULONG SessionId;

		ULARGE_INTEGER AppCompatFlags;
		ULARGE_INTEGER AppCompatFlagsUser;
		PVOID pShimData;
		PVOID AppCompatInfo;

		UNICODE_STRING CSDVersion;

		PVOID ActivationContextData;
		PVOID ProcessAssemblyStorageMap;
		PVOID SystemDefaultActivationContextData;
		PVOID SystemAssemblyStorageMap;

		SIZE_T MinimumStackCommit;

		PVOID FlsCallback;
		LIST_ENTRY FlsListHead;
		PVOID FlsBitmap;
		ULONG FlsBitmapBits[FLS_MAXIMUM_AVAILABLE / (sizeof(ULONG) * 8)];
		ULONG FlsHighIndex;

		PVOID WerRegistrationData;
		PVOID WerShipAssertPtr;
		PVOID pContextData;
		PVOID pImageHeaderHash;

		union
		{
			ULONG TracingFlags;

			struct
			{
				ULONG HeapTracingEnabled : 1;
				ULONG CritSecTracingEnabled : 1;
				ULONG LibLoaderTracingEnabled : 1;
				ULONG SpareTracingBits : 29;
			};
		};

		ULONGLONG CsrServerReadOnlySharedMemoryBase;
	} PEB, *PPEB;

	typedef struct _IMAGE_BASE_RELOCATION_EXTENDED : IMAGE_BASE_RELOCATION
	{
		struct
		{
			WORD Offset : 12;
			WORD Type : 4;
		} Records[1];
	} IMAGE_BASE_RELOCATION_EXTENDED, *PIMAGE_BASE_RELOCATION_EXTENDED;

	//Thanks http://www.debuginfo.com/articles/debuginfomatch.html & http://www.debuginfo.com/examples/src/DebugDir.cpp

	// CodeView header
	struct CV_HEADER
	{
		DWORD CvSignature; // NBxx
		LONG  Offset;      // Always 0 for NB10
	};

	// CodeView NB10 debug information
	// (used when debug information is stored in a PDB 2.00 file)
	struct CV_INFO_PDB20
	{
		CV_HEADER  Header;
		DWORD      Signature;       // seconds since 01.01.1970
		DWORD      Age;             // an always-incrementing value
		BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file
	};

	// CodeView RSDS debug information
	// (used when debug information is stored in a PDB 7.00 file)
	struct CV_INFO_PDB70
	{
		DWORD      CvSignature;
		GUID       Signature;       // unique identifier
		DWORD      Age;             // an always-incrementing value
		BYTE       PdbFileName[1];  // zero terminated string with the name of the PDB file
	};
}