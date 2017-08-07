#ifndef _NTMISC_H_e2789f20_6dd7_48d9_841c_b38cc5e46666_
#define _NTMISC_H_e2789f20_6dd7_48d9_841c_b38cc5e46666_

#include "ml/ntprocess.h"

/************************************************************************
  debugs
************************************************************************/

typedef enum
{
    SysDbgQueryModuleInformation        = 0,
    SysDbgQueryTraceInformation         = 1,
    SysDbgSetTracepoint                 = 2,
    SysDbgSetSpecialCall                = 3,
    SysDbgClearSpecialCalls             = 4,
    SysDbgQuerySpecialCalls             = 5,
    SysDbgBreakPoint                    = 6,
    SysDbgQueryVersion                  = 7,
    SysDbgReadVirtual                   = 8,
    SysDbgWriteVirtual                  = 9,
    SysDbgReadPhysical                  = 10,
    SysDbgWritePhysical                 = 11,
    SysDbgReadControlSpace              = 12,
    SysDbgWriteControlSpace             = 13,
    SysDbgReadIoSpace                   = 14,
    SysDbgWriteIoSpace                  = 15,
    SysDbgReadMsr                       = 16,
    SysDbgWriteMsr                      = 17,
    SysDbgReadBusData                   = 18,
    SysDbgWriteBusData                  = 19,
    SysDbgCheckLowMemory                = 20,
    SysDbgEnableKernelDebugger          = 21,
    SysDbgDisableKernelDebugger         = 22,
    SysDbgGetAutoKdEnable               = 23,
    SysDbgSetAutoKdEnable               = 24,
    SysDbgGetPrintBufferSize            = 25,
    SysDbgSetPrintBufferSize            = 26,
    SysDbgGetKdUmExceptionEnable        = 27,
    SysDbgSetKdUmExceptionEnable        = 28,
    SysDbgGetTriageDump                 = 29,
    SysDbgGetKdBlockEnable              = 30,
    SysDbgSetKdBlockEnable              = 31,
    SysDbgRegisterForUmBreakInfo        = 32,
    SysDbgGetUmBreakPid                 = 33,
    SysDbgClearUmBreakPid               = 34,
    SysDbgGetUmAttachPid                = 35,
    SysDbgClearUmAttachPid              = 36,

} SYSDBG_COMMAND;



/************************************************************************
  system information
************************************************************************/

typedef enum
{
    SystemBasicInformation                          = 0,
    SystemProcessorInformation                      = 1,
    SystemPerformanceInformation                    = 2,
    SystemTimeOfDayInformation                      = 3,
    SystemPathInformation                           = 4,
    SystemProcessInformation                        = 5,
    SystemCallCountInformation                      = 6,
    SystemDeviceInformation                         = 7,
    SystemProcessorPerformanceInformation           = 8,
    SystemFlagsInformation                          = 9,
    SystemCallTimeInformation                       = 10,
    SystemModuleInformation                         = 11,
    SystemLocksInformation                          = 12,
    SystemStackTraceInformation                     = 13,
    SystemPagedPoolInformation                      = 14,
    SystemNonPagedPoolInformation                   = 15,
    SystemHandleInformation                         = 16,
    SystemObjectInformation                         = 17,
    SystemPageFileInformation                       = 18,
    SystemVdmInstemulInformation                    = 19,
    SystemVdmBopInformation                         = 20,
    SystemFileCacheInformation                      = 21,
    SystemPoolTagInformation                        = 22,
    SystemInterruptInformation                      = 23,
    SystemDpcBehaviorInformation                    = 24,
    SystemFullMemoryInformation                     = 25,
    SystemLoadGdiDriverInformation                  = 26,
    SystemUnloadGdiDriverInformation                = 27,
    SystemTimeAdjustmentInformation                 = 28,
    SystemSummaryMemoryInformation                  = 29,
    SystemMirrorMemoryInformation                   = 30,
    SystemPerformanceTraceInformation               = 31,
    SystemObsolete0                                 = 32,
    SystemExceptionInformation                      = 33,
    SystemCrashDumpStateInformation                 = 34,
    SystemKernelDebuggerInformation                 = 35,
    SystemContextSwitchInformation                  = 36,
    SystemRegistryQuotaInformation                  = 37,
    SystemExtendServiceTableInformation             = 38,
    SystemPrioritySeperation                        = 39,
    SystemVerifierAddDriverInformation              = 40,
    SystemVerifierRemoveDriverInformation           = 41,
    SystemProcessorIdleInformation                  = 42,
    SystemLegacyDriverInformation                   = 43,
    SystemCurrentTimeZoneInformation                = 44,
    SystemLookasideInformation                      = 45,
    SystemTimeSlipNotification                      = 46,
    SystemSessionCreate                             = 47,
    SystemSessionDetach                             = 48,
    SystemSessionInformation                        = 49,
    SystemRangeStartInformation                     = 50,
    SystemVerifierInformation                       = 51,
    SystemVerifierThunkExtend                       = 52,
    SystemSessionProcessInformation                 = 53,
    SystemLoadGdiDriverInSystemSpace                = 54,
    SystemNumaProcessorMap                          = 55,
    SystemPrefetcherInformation                     = 56,
    SystemExtendedProcessInformation                = 57,
    SystemRecommendedSharedDataAlignment            = 58,
    SystemComPlusPackage                            = 59,
    SystemNumaAvailableMemory                       = 60,
    SystemProcessorPowerInformation                 = 61,
    SystemEmulationBasicInformation                 = 62,
    SystemEmulationProcessorInformation             = 63,
    SystemExtendedHandleInformation                 = 64,
    SystemLostDelayedWriteInformation               = 65,
    SystemBigPoolInformation                        = 66,
    SystemSessionPoolTagInformation                 = 67,
    SystemSessionMappedViewInformation              = 68,
    SystemHotpatchInformation                       = 69,
    SystemObjectSecurityMode                        = 70,
    SystemWatchdogTimerHandler                      = 71,
    SystemWatchdogTimerInformation                  = 72,
    SystemLogicalProcessorInformation               = 73,
    SystemWow64SharedInformationObsolete            = 74,
    SystemRegisterFirmwareTableInformationHandler   = 75,
    SystemFirmwareTableInformation                  = 76,
    SystemModuleInformationEx                       = 77,
    SystemVerifierTriageInformation                 = 78,
    SystemSuperfetchInformation                     = 79,
    SystemMemoryListInformation                     = 80,
    SystemFileCacheInformationEx                    = 81,
    SystemThreadPriorityClientIdInformation         = 82,
    SystemProcessorIdleCycleTimeInformation         = 83,
    SystemVerifierCancellationInformation           = 84,
    SystemProcessorPowerInformationEx               = 85,
    SystemRefTraceInformation                       = 86,
    SystemSpecialPoolInformation                    = 87,
    SystemProcessIdInformation                      = 88,
    SystemErrorPortInformation                      = 89,
    SystemBootEnvironmentInformation                = 90,
    SystemHypervisorInformation                     = 91,
    SystemVerifierInformationEx                     = 92,
    SystemTimeZoneInformation                       = 93,
    SystemImageFileExecutionOptionsInformation      = 94,
    SystemCoverageInformation                       = 95,
    SystemPrefetchPatchInformation                  = 96,
    SystemVerifierFaultsInformation                 = 97,
    SystemSystemPartitionInformation                = 98,
    SystemSystemDiskInformation                     = 99,
    SystemProcessorPerformanceDistribution          = 100,
    SystemNumaProximityNodeInformation              = 101,
    SystemDynamicTimeZoneInformation                = 102,
    SystemCodeIntegrityInformation                  = 103,
    SystemProcessorMicrocodeUpdateInformation       = 104,
    SystemProcessorBrandString                      = 105,
    SystemVirtualAddressInformation                 = 106,
    SystemLogicalProcessorAndGroupInformation       = 107,
    SystemProcessorCycleTimeInformation             = 108,
    SystemStoreInformation                          = 109,
    SystemRegistryAppendString                      = 110,
    SystemAitSamplingValue                          = 111,
    SystemVhdBootInformation                        = 112,
    SystemCpuQuotaInformation                       = 113,
    SystemSpare0                                    = 114,
    SystemSpare1                                    = 115,
    SystemLowPriorityIoInformation                  = 116,
    SystemTpmBootEntropyInformation                 = 117,
    SystemVerifierCountersInformation               = 118,
    SystemPagedPoolInformationEx                    = 119,
    SystemSystemPtesInformationEx                   = 120,
    SystemNodeDistanceInformation                   = 121,
    SystemAcpiAuditInformation                      = 122,
    SystemBasicPerformanceInformation               = 123,
    SystemQueryPerformanceCounterInformation        = 124,
    MaxSystemInfoClass                              = 125,

} SYSTEM_INFORMATION_CLASS;

typedef struct _SYSTEM_BASIC_INFORMATION
{
/* 0x000 */ ULONG       Reserved;
/* 0x004 */ ULONG       TimerResolution;
/* 0x008 */ ULONG       PageSize;
/* 0x00C */ ULONG       NumberOfPhysicalPages;
/* 0x010 */ ULONG       LowestPhysicalPageNumber;
/* 0x014 */ ULONG       HighestPhysicalPageNumber;
/* 0x018 */ ULONG       AllocationGranularity;
/* 0x01C */ ULONG_PTR   MinimumUserModeAddress;
/* 0x020 */ ULONG_PTR   MaximumUserModeAddress;
/* 0x024 */ ULONG_PTR   ActiveProcessorsAffinityMask;
/* 0x028 */ CHAR        NumberOfProcessors;

} SYSTEM_BASIC_INFORMATION, *PSYSTEM_BASIC_INFORMATION;


//
// Thread scheduling states.
//

typedef enum _THREAD_STATE {
    Initialized,
    Ready,
    Running,
    Standby,
    Terminated,
    Waiting,
    Transition,
    DeferredReady,
    GateWait
} KTHREAD_STATE;

typedef struct _SYSTEM_THREAD_INFORMATION
{
/* 0x000 */ LARGE_INTEGER   KernelTime;
/* 0x008 */ LARGE_INTEGER   UserTime;
/* 0x010 */ LARGE_INTEGER   CreateTime;
/* 0x018 */ ULONG           WaitTime;
/* 0x01c */ PVOID           StartAddress;
/* 0x020 */ CLIENT_ID       ClientId;
/* 0x028 */ LONG            Priority;
/* 0x02c */ LONG            BasePriority;
/* 0x030 */ ULONG           ContextSwitches;
/* 0x034 */ ULONG           ThreadState;
/* 0x038 */ ULONG           WaitReason;
/* 0x03C */ ULONG           PadFor16Byte;

} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;

typedef struct _SYSTEM_PROCESS_INFORMATION
{
/* 0x000 */ ULONG                       NextEntryOffset;
/* 0x004 */ ULONG                       NumberOfThreads;
/* 0x008 */ LARGE_INTEGER               WorkingSetPrivateSize;
/* 0x010 */ ULONG                       HardFaultCount;
/* 0x014 */ ULONG                       NumberOfThreadsHighWatermark;
/* 0x018 */ ULONG64                     CycleTime;
/* 0x020 */ LARGE_INTEGER               CreateTime;
/* 0x028 */ LARGE_INTEGER               UserTime;
/* 0x030 */ LARGE_INTEGER               KernelTime;
/* 0x038 */ UNICODE_STRING              ImageName;
/* 0x040 */ LONG                        BasePriority;
/* 0x044 */ ULONG_PTR                   UniqueProcessId;
/* 0x048 */ ULONG_PTR                   InheritedFromUniqueProcessId;
/* 0x04c */ ULONG                       HandleCount;
/* 0x050 */ ULONG                       SessionId;
/* 0x054 */ ULONG_PTR                   UniqueProcessKey;
/* 0x058 */ ULONG_PTR                   PeakVirtualSize;
/* 0x05c */ ULONG_PTR                   VirtualSize;
/* 0x060 */ ULONG                       PageFaultCount;
/* 0x064 */ ULONG_PTR                   PeakWorkingSetSize;
/* 0x068 */ ULONG_PTR                   WorkingSetSize;
/* 0x06c */ ULONG_PTR                   QuotaPeakPagedPoolUsage;
/* 0x070 */ ULONG_PTR                   QuotaPagedPoolUsage;
/* 0x074 */ ULONG_PTR                   QuotaPeakNonPagedPoolUsage;
/* 0x078 */ ULONG_PTR                   QuotaNonPagedPoolUsage;
/* 0x07c */ ULONG_PTR                   PagefileUsage;
/* 0x080 */ ULONG_PTR                   PeakPagefileUsage;
/* 0x084 */ ULONG_PTR                   PrivatePageCount;
/* 0x088 */ LARGE_INTEGER               ReadOperationCount;
/* 0x090 */ LARGE_INTEGER               WriteOperationCount;
/* 0x098 */ LARGE_INTEGER               OtherOperationCount;
/* 0x0a0 */ LARGE_INTEGER               ReadTransferCount;
/* 0x0a8 */ LARGE_INTEGER               WriteTransferCount;
/* 0x0b0 */ LARGE_INTEGER               OtherTransferCount;
/* 0x0b8 */ SYSTEM_THREAD_INFORMATION   Threads[1];

} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct
{
/* 0x000 */ LARGE_INTEGER  IdleProcessTime;
/* 0x008 */ LARGE_INTEGER  IoReadTransferCount;
/* 0x010 */ LARGE_INTEGER  IoWriteTransferCount;
/* 0x018 */ LARGE_INTEGER  IoOtherTransferCount;
/* 0x020 */ ULONG          IoReadOperationCount;
/* 0x024 */ ULONG          IoWriteOperationCount;
/* 0x028 */ ULONG          IoOtherOperationCount;
/* 0x02c */ ULONG          AvailablePages;
/* 0x030 */ ULONG          CommittedPages;
/* 0x034 */ ULONG          CommitLimit;
/* 0x038 */ ULONG          PeakCommitment;
/* 0x03c */ ULONG          PageFaultCount;
/* 0x040 */ ULONG          CopyOnWriteCount;
/* 0x044 */ ULONG          TransitionCount;
/* 0x048 */ ULONG          CacheTransitionCount;
/* 0x04c */ ULONG          DemandZeroCount;
/* 0x050 */ ULONG          PageReadCount;
/* 0x054 */ ULONG          PageReadIoCount;
/* 0x058 */ ULONG          CacheReadCount;
/* 0x05c */ ULONG          CacheIoCount;
/* 0x060 */ ULONG          DirtyPagesWriteCount;
/* 0x064 */ ULONG          DirtyWriteIoCount;
/* 0x068 */ ULONG          MappedPagesWriteCount;
/* 0x06c */ ULONG          MappedWriteIoCount;
/* 0x070 */ ULONG          PagedPoolPages;
/* 0x074 */ ULONG          NonPagedPoolPages;
/* 0x078 */ ULONG          PagedPoolAllocs;
/* 0x07c */ ULONG          PagedPoolFrees;
/* 0x080 */ ULONG          NonPagedPoolAllocs;
/* 0x084 */ ULONG          NonPagedPoolFrees;
/* 0x088 */ ULONG          FreeSystemPtes;
/* 0x08c */ ULONG          ResidentSystemCodePage;
/* 0x090 */ ULONG          TotalSystemDriverPages;
/* 0x094 */ ULONG          TotalSystemCodePages;
/* 0x098 */ ULONG          NonPagedPoolLookasideHits;
/* 0x09c */ ULONG          PagedPoolLookasideHits;
/* 0x0a0 */ ULONG          AvailablePagedPoolPages;
/* 0x0a4 */ ULONG          ResidentSystemCachePage;
/* 0x0a8 */ ULONG          ResidentPagedPoolPage;
/* 0x0ac */ ULONG          ResidentSystemDriverPage;
/* 0x0b0 */ ULONG          CcFastReadNoWait;
/* 0x0b4 */ ULONG          CcFastReadWait;
/* 0x0b8 */ ULONG          CcFastReadResourceMiss;
/* 0x0bc */ ULONG          CcFastReadNotPossible;
/* 0x0c0 */ ULONG          CcFastMdlReadNoWait;
/* 0x0c4 */ ULONG          CcFastMdlReadWait;
/* 0x0c8 */ ULONG          CcFastMdlReadResourceMiss;
/* 0x0cc */ ULONG          CcFastMdlReadNotPossible;
/* 0x0d0 */ ULONG          CcMapDataNoWait;
/* 0x0d4 */ ULONG          CcMapDataWait;
/* 0x0d8 */ ULONG          CcMapDataNoWaitMiss;
/* 0x0dc */ ULONG          CcMapDataWaitMiss;
/* 0x0e0 */ ULONG          CcPinMappedDataCount;
/* 0x0e4 */ ULONG          CcPinReadNoWait;
/* 0x0e8 */ ULONG          CcPinReadWait;
/* 0x0ec */ ULONG          CcPinReadNoWaitMiss;
/* 0x0f0 */ ULONG          CcPinReadWaitMiss;
/* 0x0f4 */ ULONG          CcCopyReadNoWait;
/* 0x0f8 */ ULONG          CcCopyReadWait;
/* 0x0fc */ ULONG          CcCopyReadNoWaitMiss;
/* 0x100 */ ULONG          CcCopyReadWaitMiss;
/* 0x104 */ ULONG          CcMdlReadNoWait;
/* 0x108 */ ULONG          CcMdlReadWait;
/* 0x10c */ ULONG          CcMdlReadNoWaitMiss;
/* 0x110 */ ULONG          CcMdlReadWaitMiss;
/* 0x114 */ ULONG          CcReadAheadIos;
/* 0x118 */ ULONG          CcLazyWriteIos;
/* 0x11c */ ULONG          CcLazyWritePages;
/* 0x120 */ ULONG          CcDataFlushes;
/* 0x124 */ ULONG          CcDataPages;
/* 0x128 */ ULONG          ContextSwitches;
/* 0x12c */ ULONG          FirstLevelTbFills;
/* 0x130 */ ULONG          SecondLevelTbFills;
/* 0x134 */ ULONG          SystemCalls;
/* 0x138 */ ULONG64        CcTotalDirtyPages;
/* 0x140 */ ULONG64        CcDirtyPageThreshold;

} SYSTEM_PERFORMANCE_INFORMATION;

typedef struct
{
/* 0x000 */ LARGE_INTEGER   BootTime;
/* 0x008 */ LARGE_INTEGER   CurrentTime;
/* 0x010 */ LARGE_INTEGER   TimeZoneBias;
/* 0x018 */ ULONG           TimeZoneId;
/* 0x01c */ ULONG           Reserved;
/* 0x020 */ ULONG64         BootTimeBias;
/* 0x028 */ ULONG64         SleepTimeBias;

} SYSTEM_TIMEOFDAY_INFORMATION;

typedef struct
{
/* 0x000 */ USHORT  ProcessorArchitecture;
/* 0x002 */ USHORT  ProcessorLevel;
/* 0x004 */ USHORT  ProcessorRevision;
/* 0x006 */ USHORT  Reserved;
/* 0x008 */ ULONG   ProcessorFeatureBits;

} SYSTEM_PROCESSOR_INFORMATION;

typedef struct
{
/* 0x000 */ LARGE_INTEGER   IdleTime;
/* 0x008 */ LARGE_INTEGER   KernelTime;
/* 0x010 */ LARGE_INTEGER   UserTime;
/* 0x018 */ LARGE_INTEGER   DpcTime;
/* 0x020 */ LARGE_INTEGER   InterruptTime;
/* 0x028 */ ULONG           InterruptCount;

} SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION;

typedef struct
{
/* 0x000 */ ULONG Length;
/* 0x004 */ ULONG NumberOfTables;
//          ULONG TableLimit[1]

} SYSTEM_CALL_COUNT_INFORMATION;
/*
typedef struct
{
    PVOID   Reserved1;
    PVOID   Reserved2;
    PVOID   ImageBaseAddress;
    ULONG   ImageSize;
    ULONG   Flags;
    USHORT  Id;
    USHORT  Rank;
    USHORT  w018;
    USHORT  NameOffset;
    CHAR    Name[MAXIMUM_FILENAME_LENGTH];

} SYSTEM_MODULE, *PSYSTEM_MODULE;

typedef struct
{
    USHORT  NextEntryOffset;
    USHORT  Dummy;
    ULONG   Reserved1;
    ULONG   Reserved2;
    PVOID   ImageBaseAddress;
    ULONG   ImageSize;
    ULONG   Flags;
    WORD    Id;
    WORD    Rank;
    WORD    w018;
    WORD    NameOffset;
    CHAR    Name[MAXIMUM_FILENAME_LENGTH];
    ULONG   Unknown;
    ULONG   Reserved3;
    ULONG   Reserved4;

} SYSTEM_MODULE_INFORMATION_EX, *PSYSTEM_MODULE_INFORMATION_EX;

typedef struct
{
    ULONG           ModulesCount;
    SYSTEM_MODULE   Modules[1];
} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;
*/
typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO
{
    USHORT  UniqueProcessId;
    USHORT  CreatorBackTraceIndex;
    UCHAR   ObjectTypeIndex;
    UCHAR   HandleAttributes;
    USHORT  HandleValue;
    PVOID   Object;
    ULONG   GrantedAccess;

} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION
{
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];

} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX
{
    PVOID       Object;
    ULONG_PTR   UniqueProcessId;
    HANDLE      HandleValue;
    ULONG       GrantedAccess;
    USHORT      CreatorBackTraceIndex;
    USHORT      ObjectTypeIndex;
    ULONG       HandleAttributes;
    ULONG       Reserved;

} SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO_EX;

typedef struct _SYSTEM_HANDLE_INFORMATION_EX
{
    ULONG_PTR NumberOfHandles;
    ULONG_PTR Reserved;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO_EX Handles[1];

} SYSTEM_HANDLE_INFORMATION_EX, *PSYSTEM_HANDLE_INFORMATION_EX;

typedef struct
{
/* 0x000 */ ULONG CurrentSize;
/* 0x004 */ ULONG PeakSize;
/* 0x008 */ ULONG PageFaultCount;
/* 0x00c */ ULONG MinimumWorkingSet;
/* 0x010 */ ULONG MaximumWorkingSet;
/* 0x014 */ ULONG CurrentSizeIncludingTransitionInPages;
/* 0x018 */ ULONG PeakSizeIncludingTransitionInPages;
/* 0x01c */ ULONG TransitionRePurposeCount;
/* 0x020 */ ULONG Flags;

} SYSTEM_FILECACHE_INFORMATION;

typedef struct
{
    BOOLEAN DebuggerEnabled;
    BOOLEAN DebuggerNotPresent;

} SYSTEM_KERNEL_DEBUGGER_INFORMATION, *PSYSTEM_KERNEL_DEBUGGER_INFORMATION;

NATIVE_API
NTSTATUS
NTAPI
NtQueryLicenseValue(
    IN  PUNICODE_STRING Name,
    IN  PULONG          Type OPTIONAL,
    OUT PVOID           Buffer,
    IN  ULONG           Length,
    OUT PULONG          DataLength
);

NATIVE_API
BOOL
NTAPI
RtlQueryPerformanceCounter(
    OUT PLARGE_INTEGER PerformanceCount
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryPerformanceCounter(
    IN PLARGE_INTEGER   Counter,
    IN PLARGE_INTEGER   Frequency
);

NATIVE_API
NTSTATUS
NTAPI
NtQuerySystemInformation(
    IN  SYSTEM_INFORMATION_CLASS    SystemInformationClass,
    OUT PVOID                       SystemInformation,
    IN  ULONG_PTR                   SystemInformationLength,
    OUT PULONG                      ReturnLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
ZwQuerySystemInformation(
    IN  SYSTEM_INFORMATION_CLASS    SystemInformationClass,
    OUT PVOID                       SystemInformation,
    IN  ULONG_PTR                   SystemInformationLength,
    OUT PULONG                      ReturnLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtAddAtom(
    PCWSTR  AtomString,
    ULONG   BytesInString,
    ATOM   *Atom
);

NATIVE_API
NTSTATUS
NTAPI
ZwAddAtom(
    PCWSTR  AtomString,
    ULONG   BytesInString,
    ATOM   *Atom
);

NATIVE_API
NTSTATUS
NTAPI
NtDeleteAtom(
    ATOM Atom
);

NATIVE_API
NTSTATUS
NTAPI
ZwDeleteAtom(
    ATOM Atom
);

NATIVE_API
NTSTATUS
NTAPI
NtSystemDebugControl(
    IN  SYSDBG_COMMAND  Command,
    IN  PVOID           InputBuffer,
    IN  ULONG_PTR       InputBufferLength,
    OUT PVOID           OutputBuffer,
    IN  ULONG_PTR       OutputBufferLength,
    OUT PULONG          ReturnLength
);

#if !ML_KERNEL_MODE

NATIVE_API
VOID
NTAPI
LdrInitializeThunk(
    PCONTEXT    ThreadContext,
    PVOID       NtdllBase
);

NATIVE_API
VOID
FASTCALL
KiFastSystemCall(
    VOID
);

NATIVE_API
VOID
NTAPI
KiUserCallbackDispatcher(
    ULONG_PTR,
    ULONG_PTR   apfnDispatchIndex,
    PVOID       Parameter
);

NATIVE_API
ULONG64
NTAPI
NtGetTickCount(
    VOID
);

NATIVE_API
ULONG
NTAPI
RtlRandom(
    IN OUT PULONG Seed
);

NATIVE_API
ULONG
NTAPI
RtlRandomEx(
    IN OUT PULONG Random
);

NATIVE_API
NTSTATUS
NTAPI
RtlExpandEnvironmentStrings_U(
    IN  PVOID           Environment OPTIONAL,
    IN  PUNICODE_STRING SourceString,
    OUT PUNICODE_STRING DestinationString,
    OUT PULONG          DestinationBufferLength OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlCreateSecurityDescriptor(
    IN OUT  PSECURITY_DESCRIPTOR    SecurityDescriptor,
    IN      ULONG                   Revision
);

NATIVE_API
NTSTATUS
NTAPI
RtlCreateAcl(
    IN PACL     Acl,
    IN ULONG    AclLength,
    IN ULONG    AclRevision
);

NATIVE_API
NTSTATUS
NTAPI
RtlAddAccessDeniedAce(
    IN OUT  PACL            Acl,
    IN      ULONG           AceRevision,
    IN      ACCESS_MASK     AccessMask,
    IN      PSID            Sid
);

NATIVE_API
NTSTATUS
NTAPI
RtlAddAccessAllowedAce(
    IN OUT  PACL            Acl,
    IN      ULONG           AceRevision,
    IN      ACCESS_MASK     AccessMask,
    IN      PSID            Sid
);

NATIVE_API
NTSTATUS
NTAPI
RtlSetOwnerSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID                 Owner OPTIONAL,
    IN BOOLEAN              OwnerDefaulted OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlSetGroupSecurityDescriptor(
    IN PSECURITY_DESCRIPTOR SecurityDescriptor,
    IN PSID                 Group OPTIONAL,
    IN BOOLEAN              GroupDefaulted OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlSetDaclSecurityDescriptor(
    IN OUT  PSECURITY_DESCRIPTOR    SecurityDescriptor,
    IN      BOOLEAN                 DaclPresent,
    IN      PACL                    Dacl OPTIONAL,
    IN      BOOLEAN                 DaclDefaulted OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlFormatMessage(
    LPWSTR  Message,              // [In] Message to format
    UCHAR   MaxWidth,             // [In] Maximum width in characters of each output line
    BOOLEAN IgnoreInserts,        // [In] Whether to copy the message without processing inserts
    BOOLEAN Ansi,                 // [In] Whether Arguments may have ANSI strings
    BOOLEAN ArgumentIsArray,      // [In] Whether Arguments is actually an array rather than a va_list *
    va_list *Arguments,
    LPWSTR  Buffer,               // [Out] Buffer to store processed message in
    ULONG   BufferSize            // [In] Size of Buffer (in bytes?)
);

NATIVE_API
BOOLEAN
NTAPI
RtlDosPathNameToNtPathName_U(
    IN  PCWSTR                  DosName,
    OUT PUNICODE_STRING         NtName,
    OUT PCWSTR*                 DosFilePath OPTIONAL,
    OUT PRTL_RELATIVE_NAME_U    FileName OPTIONAL
);

NATIVE_API
VOID
NTAPI
RtlReleaseRelativeName(
    IN OUT PRTL_RELATIVE_NAME_U RelativeName
);

NATIVE_API
NTSTATUS
NTAPI
RtlNtPathNameToDosPathName(
    IN      ULONG                       Flags,
    IN OUT  PRTL_UNICODE_STRING_BUFFER  Path,
    OUT     PULONG                      Disposition OPTIONAL,
    OUT     PWSTR*                      FilePart OPTIONAL
);

NATIVE_API
ULONG
NTAPI
RtlGetFullPathName_U(
    LPCWSTR FileName,
    ULONG   BufferSizeInBytes,
    LPWSTR  Buffer,
    LPWSTR *FilePart
);

NATIVE_API
BOOLEAN
NTAPI
RtlDosPathNameToRelativeNtPathName_U(
    IN  PWSTR                   DosFileName,
    OUT PUNICODE_STRING         NtFileName,
    OUT PWSTR*                  FilePart OPTIONAL,
    OUT PRTL_RELATIVE_NAME_U    RelativeName OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlGetVersion(
    IN OUT PRTL_OSVERSIONINFOW lpVersionInformation
);

NATIVE_API
ULONG
NTAPI
RtlNtStatusToDosError(
    NTSTATUS Status
);

NATIVE_API
NTSTATUS
NTAPI
RtlGetLastWin32Error(
    VOID
);

NATIVE_API
VOID
NTAPI
RtlSetLastWin32Error(
    IN LONG ErrorCode
);

NATIVE_API
VOID
NTAPI
RtlSetLastWin32ErrorAndNtStatusFromNtStatus(
    IN NTSTATUS NtStatus
);

NATIVE_API
NTSTATUS
NTAPI
RtlFindMessage(
    PVOID                    DllHandle,
    ULONG                    MessageTableId,
    ULONG                    MessageLanguageId,
    ULONG                    MessageId,
    PMESSAGE_RESOURCE_ENTRY *MessageEntry
);

typedef struct
{
    ULONG Count[2];
    ULONG State[4];
    UCHAR Buffer[64];
    ULONG Digest[4];

} RTL_MD5_CTX, *PRTL_MD5_CTX;

NATIVE_API
VOID
NTAPI
MD5Init(
    OUT PRTL_MD5_CTX Context
);

NATIVE_API
VOID
NTAPI
MD5Update(
    IN OUT  PRTL_MD5_CTX    Context,
    IN      PVOID           Buffer,
    IN      ULONG           BufferSize
);

NATIVE_API
VOID
NTAPI
MD5Final(
    IN OUT  PRTL_MD5_CTX Context
);

typedef struct
{
    ULONG Unknown[6];
    ULONG State[5];
    ULONG Count[2];
    UCHAR Buffer[64];

} RTL_SHA_CTX, *PRTL_SHA_CTX;

typedef struct
{
    ULONG Digest[5];
} RTL_SHA_DIG, *PRTL_SHA_DIG;

NATIVE_API
VOID
NTAPI
A_SHAInit(
    PRTL_SHA_CTX ShaContext
);

NATIVE_API
VOID
NTAPI
A_SHAUpdate(
    PRTL_SHA_CTX    ShaContext,
    PVOID           Buffer,
    ULONG           BufferSize
);

NATIVE_API
VOID
NTAPI
A_SHAFinal(
    PRTL_SHA_CTX ShaContext,
    PRTL_SHA_DIG Digest
);

/************************************************************************
  handles
************************************************************************/

NATIVE_API
VOID
NTAPI
RtlInitializeHandleTable(
    IN  ULONG               TableSize,
    IN  ULONG               HandleSize,
    OUT PRTL_HANDLE_TABLE   HandleTable
);

NATIVE_API
PRTL_HANDLE_TABLE_ENTRY
NTAPI
RtlAllocateHandle(
    IN OUT  PRTL_HANDLE_TABLE   HandleTable,
    OUT     PULONG              Index
);

NATIVE_API
BOOLEAN
NTAPI
RtlFreeHandle(
    IN OUT  PRTL_HANDLE_TABLE       HandleTable,
    IN      PRTL_HANDLE_TABLE_ENTRY Handle
);

NATIVE_API
BOOLEAN
NTAPI
RtlIsValidHandle(
    IN PRTL_HANDLE_TABLE        HandleTable,
    IN PRTL_HANDLE_TABLE_ENTRY  Handle
);

NATIVE_API
BOOLEAN
NTAPI
RtlIsValidIndexHandle(
    IN  PRTL_HANDLE_TABLE        HandleTable,
    IN  ULONG                    Index,
    OUT PRTL_HANDLE_TABLE_ENTRY *Handle
);

NATIVE_API
VOID
NTAPI
RtlDestroyHandleTable(
    IN OUT PRTL_HANDLE_TABLE HandleTable
);

#define COMPRESSION_FORMAT_NONE          (0x0000)   // winnt
#define COMPRESSION_FORMAT_DEFAULT       (0x0001)   // winnt
#define COMPRESSION_FORMAT_LZNT1         (0x0002)   // winnt
#define COMPRESSION_FORMAT_XPRESS        (0x0003)
#define COMPRESSION_FORMAT_XPRESS_HUFF   (0x0004)

#define COMPRESSION_ENGINE_STANDARD      (0x0000)   // winnt
#define COMPRESSION_ENGINE_MAXIMUM       (0x0100)   // winnt
#define COMPRESSION_ENGINE_HIBER         (0x0200)   // winnt

NATIVE_API
NTSTATUS
NTAPI
RtlGetCompressionWorkSpaceSize(
    IN  USHORT CompressionFormatAndEngine,
    OUT PULONG CompressBufferWorkSpaceSize,
    OUT PULONG CompressFragmentWorkSpaceSize
);

NATIVE_API
NTSTATUS
NTAPI
RtlCompressBuffer(
    IN  USHORT  CompressionFormatAndEngine,
    IN  PVOID   UncompressedBuffer,
    IN  ULONG   UncompressedBufferSize,
    OUT PVOID   CompressedBuffer,
    IN  ULONG   CompressedBufferSize,
    IN  ULONG   UncompressedChunkSize,
    OUT PULONG  FinalCompressedSize,
    IN  PVOID   WorkSpace
);

NATIVE_API
NTSTATUS
NTAPI
RtlDecompressBuffer(
    IN  USHORT  CompressionFormat,
    OUT PVOID   UncompressedBuffer,
    IN  ULONG   UncompressedBufferSize,
    IN  PVOID   CompressedBuffer,
    IN  ULONG   CompressedBufferSize,
    OUT PULONG  FinalUncompressedSize
);

#endif // !ML_KERNEL_MODE

#endif // _NTMISC_H_e2789f20_6dd7_48d9_841c_b38cc5e46666_
