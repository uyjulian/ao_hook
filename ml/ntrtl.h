#ifndef _NTRTL_H_0fd28a55_9a32_4533_8006_a0469dd77ef2_
#define _NTRTL_H_0fd28a55_9a32_4533_8006_a0469dd77ef2_


typedef struct _RTL_PROCESS_MODULE_INFORMATION_EX
{
/* 0x000 */ USHORT                              NextOffset;
/* 0x004 */ RTL_PROCESS_MODULE_INFORMATION      BaseInfo;
/* 0x120 */ ULONG                               ImageChecksum;
/* 0x124 */ ULONG                               TimeDateStamp;
/* 0x128 */ PVOID                               DefaultBase;

} RTL_PROCESS_MODULE_INFORMATION_EX, *PRTL_PROCESS_MODULE_INFORMATION_EX;

typedef struct _RTL_PROCESS_BACKTRACE_INFORMATION
{
/* 0x000 */ PCHAR                               SymbolicBackTrace;
/* 0x004 */ ULONG                               TraceCount;
/* 0x008 */ USHORT                              Index;
/* 0x00A */ USHORT                              Depth;
/* 0x00C */ PVOID                               BackTrace[32];

} RTL_PROCESS_BACKTRACE_INFORMATION, *PRTL_PROCESS_BACKTRACE_INFORMATION;

typedef struct _RTL_PROCESS_BACKTRACES
{
/* 0x000 */ ULONG                               CommittedMemory;
/* 0x004 */ ULONG                               ReservedMemory;
/* 0x008 */ ULONG                               NumberOfBackTraceLookups;
/* 0x00C */ ULONG                               NumberOfBackTraces;
/* 0x010 */ RTL_PROCESS_BACKTRACE_INFORMATION   BackTraces[1];

} RTL_PROCESS_BACKTRACES, *PRTL_PROCESS_BACKTRACES;

typedef struct _RTL_HEAP_TAG
{
/* 0x000 */ ULONG                               NumberOfAllocations;
/* 0x004 */ ULONG                               NumberOfFrees;
/* 0x008 */ ULONG_PTR                           BytesAllocated;
/* 0x00C */ USHORT                              TagIndex;
/* 0x00E */ USHORT                              CreatorBackTraceIndex;
/* 0x010 */ WCHAR                               TagName[24];

} RTL_HEAP_TAG, *PRTL_HEAP_TAG;

typedef struct _RTL_HEAP_ENTRY
{
/* 0x000 */ ULONG_PTR                           Size;
/* 0x004 */ USHORT                              Flags;
/* 0x006 */ USHORT                              AllocatorBackTraceIndex;

    union
    {
        struct
        {
/* 0x008 */ ULONG_PTR                           Settable;
/* 0x00C */ ULONG                               Tag;
        } s1;

        struct
        {
/* 0x008 */ ULONG_PTR                           CommittedSize;
/* 0x00C */ PVOID                               FirstBlock;
        } s2;
    } u;

} RTL_HEAP_ENTRY, *PRTL_HEAP_ENTRY;

typedef struct _RTL_HEAP_INFORMATION
{
/* 0x000 */ PVOID                               BaseAddress;
/* 0x004 */ ULONG                               Flags;
/* 0x008 */ USHORT                              EntryOverhead;
/* 0x00A */ USHORT                              CreatorBackTraceIndex;
/* 0x00C */ ULONG_PTR                           BytesAllocated;
/* 0x010 */ ULONG_PTR                           BytesCommitted;
/* 0x014 */ ULONG                               NumberOfTags;
/* 0x018 */ ULONG                               NumberOfEntries;
/* 0x01C */ ULONG                               NumberOfPseudoTags;
/* 0x020 */ ULONG                               PseudoTagGranularity;
/* 0x024 */ ULONG                               Reserved[5];
/* 0x038 */ PRTL_HEAP_TAG                       Tags;
/* 0x03C */ PRTL_HEAP_ENTRY                     Entries;

} RTL_HEAP_INFORMATION, *PRTL_HEAP_INFORMATION;

typedef struct _RTL_PROCESS_HEAPS
{
/* 0x000 */ ULONG                               NumberOfHeaps;
/* 0x004 */ RTL_HEAP_INFORMATION                Heaps[1];

} RTL_PROCESS_HEAPS, *PRTL_PROCESS_HEAPS;

typedef struct _RTL_PROCESS_LOCK_INFORMATION
{
/* 0x000 */ PVOID                               Address;
/* 0x004 */ USHORT                              Type;
/* 0x006 */ USHORT                              CreatorBackTraceIndex;
/* 0x008 */ PVOID                               OwningThread;   // from the thread's ClientId->UniqueThread
/* 0x00C */ LONG                                LockCount;
/* 0x010 */ ULONG                               ContentionCount;
/* 0x014 */ ULONG                               EntryCount;

    //
    // The following fields are only valid for Type == RTL_CRITSECT_TYPE
    //

/* 0x018 */ LONG                                RecursionCount;

    //
    // The following fields are only valid for Type == RTL_RESOURCE_TYPE
    //

/* 0x01C */ ULONG                               NumberOfWaitingShared;
/* 0x020 */ ULONG                               NumberOfWaitingExclusive;

} RTL_PROCESS_LOCK_INFORMATION, *PRTL_PROCESS_LOCK_INFORMATION;

typedef struct _RTL_PROCESS_LOCKS
{
/* 0x000 */ ULONG                               NumberOfLocks;
/* 0x004 */ RTL_PROCESS_LOCK_INFORMATION        Locks[1];

} RTL_PROCESS_LOCKS, *PRTL_PROCESS_LOCKS;

typedef struct _RTL_PROCESS_VERIFIER_OPTIONS
{
/* 0x000 */ ULONG                               SizeStruct;
/* 0x004 */ ULONG                               Option;
/* 0x008 */ UCHAR                               OptionData[1];

} RTL_PROCESS_VERIFIER_OPTIONS, *PRTL_PROCESS_VERIFIER_OPTIONS;

typedef struct _RTL_DEBUG_INFORMATION
{
/* 0x000 */ PVOID                               SectionHandleClient;
/* 0x004 */ PVOID                               ViewBaseClient;
/* 0x008 */ PVOID                               ViewBaseTarget;
/* 0x00C */ ULONG_PTR                           ViewBaseDelta;
/* 0x010 */ PVOID                               EventPairClient;
/* 0x014 */ PVOID                               EventPairTarget;
/* 0x018 */ PVOID                               TargetProcessId;
/* 0x01C */ PVOID                               TargetThreadHandle;
/* 0x020 */ ULONG                               Flags;
/* 0x024 */ ULONG_PTR                           OffsetFree;
/* 0x028 */ ULONG_PTR                           CommitSize;
/* 0x02C */ ULONG_PTR                           ViewSize;
            union
            {
/* 0x030 */     PRTL_PROCESS_MODULES                Modules;
/* 0x030 */     PRTL_PROCESS_MODULE_INFORMATION_EX  ModulesEx;
            };

/* 0x034 */ PRTL_PROCESS_BACKTRACES             BackTraces;
/* 0x038 */ PRTL_PROCESS_HEAPS                  Heaps;
/* 0x03C */ PRTL_PROCESS_LOCKS                  Locks;
/* 0x040 */ PVOID                               SpecificHeap;
/* 0x044 */ PVOID                               TargetProcessHandle;
/* 0x048 */ PRTL_PROCESS_VERIFIER_OPTIONS       VerifierOptions;
/* 0x04C */ PVOID                               ProcessHeap;
/* 0x050 */ PVOID                               CriticalSectionHandle;
/* 0x054 */ PVOID                               CriticalSectionOwnerThread;
/* 0x058 */ PVOID                               Reserved[4];

} RTL_DEBUG_INFORMATION, *PRTL_DEBUG_INFORMATION;

NATIVE_API
PRTL_DEBUG_INFORMATION
NTAPI
RtlCreateQueryDebugBuffer(
    IN  ULONG   MaximumCommit OPTIONAL,
    IN  BOOLEAN UseEventPair
);

NATIVE_API
NTSTATUS
NTAPI
RtlDestroyQueryDebugBuffer(
    IN  PRTL_DEBUG_INFORMATION Buffer
);

NATIVE_API
PVOID
NTAPI
RtlCommitDebugInfo(
    IN OUT  PRTL_DEBUG_INFORMATION Buffer,
    IN      ULONG_PTR Size
);

NATIVE_API
VOID
NTAPI
RtlDeCommitDebugInfo(
    IN OUT  PRTL_DEBUG_INFORMATION Buffer,
    IN      PVOID p,
    IN      ULONG_PTR Size
);

#define RTL_QUERY_PROCESS_MODULES           0x00000001
#define RTL_QUERY_PROCESS_BACKTRACES        0x00000002
#define RTL_QUERY_PROCESS_HEAP_SUMMARY      0x00000004
#define RTL_QUERY_PROCESS_HEAP_TAGS         0x00000008
#define RTL_QUERY_PROCESS_HEAP_ENTRIES      0x00000010
#define RTL_QUERY_PROCESS_LOCKS             0x00000020
#define RTL_QUERY_PROCESS_MODULES32         0x00000040
#define RTL_QUERY_PROCESS_VERIFIER_OPTIONS  0x00000080 // rev
#define RTL_QUERY_PROCESS_MODULESEX         0x00000100 // rev
#define RTL_QUERY_PROCESS_HEAP_ENTRIES_EX   0x00000200 // ?
#define RTL_QUERY_PROCESS_CS_OWNER          0x00000400 // rev
#define RTL_QUERY_PROCESS_NONINVASIVE       0x80000000

NATIVE_API
NTSTATUS
NTAPI
RtlQueryProcessDebugInformation(
    IN      ULONG_PTR               UniqueProcessId,
    IN      ULONG                   Flags,
    IN OUT  PRTL_DEBUG_INFORMATION  Buffer
);

#if ML_USER_MODE

NATIVE_API
NTSTATUS
NTAPI
RtlQueryInformationActiveActivationContext(
    ACTIVATION_CONTEXT_INFO_CLASS   InfoClass,
    PVOID                           Buffer,
    ULONG_PTR                       BufferSize OPTIONAL,
    PULONG_PTR                      ReturendLength OPTIONAL
);

typedef struct _TIME_FIELDS {
    SHORT Year;        // range [1601...]
    SHORT Month;       // range [1..12]
    SHORT Day;         // range [1..31]
    SHORT Hour;        // range [0..23]
    SHORT Minute;      // range [0..59]
    SHORT Second;      // range [0..59]
    SHORT Milliseconds;// range [0..999]
    SHORT Weekday;     // range [0..6] == [Sunday..Saturday]
} TIME_FIELDS, *PTIME_FIELDS;

#endif // r3

typedef struct _RTL_TIME_ZONE_INFORMATION {
    LONG        Bias;
    WCHAR       StandardName[32];
    TIME_FIELDS StandardStart;
    LONG        StandardBias;
    WCHAR       DaylightName[32];
    TIME_FIELDS DaylightStart;
    LONG        DaylightBias;
} RTL_TIME_ZONE_INFORMATION, *PRTL_TIME_ZONE_INFORMATION;

NATIVE_API
PIMAGE_NT_HEADERS
NTAPI
RtlImageNtHeader(
    PVOID Base
);

NATIVE_API
NTSTATUS
NTAPI
RtlImageNtHeaderEx(
    IN  ULONG               Flags,
    IN  PVOID               Base,
    IN  ULONG64             Size,
    OUT PIMAGE_NT_HEADERS*  OutHeaders
);

#define INITIAL_CRC32   ULONG_PTR_MAX

NATIVE_API
ULONG
NTAPI
RtlComputeCrc32(
    IN ULONG32  PartialCrc,
    IN PVOID    Buffer,
    IN ULONG    Length
);

NATIVE_API
VOID
NTAPI
RtlAcquirePebLock(
    VOID
);

NATIVE_API
VOID
NTAPI
RtlReleasePebLock(
    VOID
);

NATIVE_API
NTSTATUS
NTAPI
RtlSetEnvironmentVar(
    IN PWSTR*       Environment OPTIONAL,
    IN  PWSTR       Name,
    IN  ULONG_PTR   NameLength,
    IN  PWSTR       Value,
    IN  ULONG_PTR   ValueLength
);

NATIVE_API
NTSTATUS
NTAPI
RtlSetEnvironmentVariable(
    IN  PVOID*          Environment OPTIONAL,
    IN  PUNICODE_STRING Name,
    IN  PUNICODE_STRING Value
);

NATIVE_API
NTSTATUS
NTAPI
RtlQueryEnvironmentVariable(
    IN  PVOID       Environment OPTIONAL,
    IN  PWSTR       Name,
    IN  ULONG_PTR   NameLength,
    OUT PWSTR       Value,
    IN  ULONG_PTR   ValueLength,
    OUT PULONG_PTR  ReturnLength
);

#define RTL_DOS_SEARCH_PATH_FLAG_APPLY_ISOLATION_REDIRECTION 0x00000001
#define RTL_DOS_SEARCH_PATH_FLAG_DISALLOW_DOT_RELATIVE_PATH_SEARCH 0x00000002
#define RTL_DOS_SEARCH_PATH_FLAG_APPLY_DEFAULT_EXTENSION_WHEN_NOT_RELATIVE_PATH_EVEN_IF_FILE_HAS_EXTENSION 0x00000004

#define RTL_DOS_SEARCH_PATH_FLAG_ALL    (RTL_DOS_SEARCH_PATH_FLAG_APPLY_ISOLATION_REDIRECTION | RTL_DOS_SEARCH_PATH_FLAG_DISALLOW_DOT_RELATIVE_PATH_SEARCH | RTL_DOS_SEARCH_PATH_FLAG_APPLY_DEFAULT_EXTENSION_WHEN_NOT_RELATIVE_PATH_EVEN_IF_FILE_HAS_EXTENSION)

NATIVE_API
ULONG
NTAPI
RtlDosSearchPath_U(
    IN  PWSTR   Path,
    IN  PWSTR   FileName,
    IN  PWSTR   Extension OPTIONAL,
    IN  ULONG   BufferLength,
    OUT PWSTR   Buffer,
    OUT PWSTR*  FilePart OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlDosSearchPath_Ustr(
    IN  ULONG               Flags,
    IN  PUNICODE_STRING     Path,
    IN  PUNICODE_STRING     FileName,
    IN  PUNICODE_STRING     DefaultExtension OPTIONAL,
    OUT PUNICODE_STRING     StaticString OPTIONAL,
    OUT PUNICODE_STRING     DynamicString OPTIONAL,
    OUT PCUNICODE_STRING*   FullFileNameOut OPTIONAL,
    OUT PULONG_PTR          FilePartPrefixCch OPTIONAL,
    OUT PULONG_PTR          BytesRequired OPTIONAL
);

NATIVE_API
BOOLEAN
NTAPI
RtlDoesFileExists_U(
    IN PWSTR FileName
);


#if ML_USER_MODE

// Resources

typedef struct _RTL_RESOURCE
{
    RTL_CRITICAL_SECTION CriticalSection;

    HANDLE  SharedSemaphore;
    ULONG   NumberOfWaitingShared;
    HANDLE  ExclusiveSemaphore;
    ULONG   NumberOfWaitingExclusive;

    LONG    NumberOfActive; // negative: exclusive acquire; zero: not acquired; positive: shared acquire(s)
    HANDLE  ExclusiveOwnerThread;

    ULONG   Flags; // RTL_RESOURCE_FLAG_*

    PRTL_RESOURCE_DEBUG DebugInfo;

} RTL_RESOURCE, *PRTL_RESOURCE;

#define RTL_RESOURCE_FLAG_LONG_TERM ((ULONG)0x00000001)

NATIVE_API
VOID
NTAPI
RtlInitializeResource(
    OUT PRTL_RESOURCE Resource
);

NATIVE_API
VOID
NTAPI
RtlDeleteResource(
    IN OUT PRTL_RESOURCE Resource
);

NATIVE_API
BOOLEAN
NTAPI
RtlAcquireResourceShared(
    IN OUT  PRTL_RESOURCE Resource,
    IN      BOOLEAN Wait
);

NATIVE_API
BOOLEAN
NTAPI
RtlAcquireResourceExclusive(
    IN OUT  PRTL_RESOURCE Resource,
    IN      BOOLEAN Wait
);

NATIVE_API
VOID
NTAPI
RtlReleaseResource(
    IN OUT PRTL_RESOURCE Resource
);

NATIVE_API
VOID
NTAPI
RtlConvertSharedToExclusive(
    IN OUT PRTL_RESOURCE Resource
);

NATIVE_API
VOID
NTAPI
RtlConvertExclusiveToShared(
    IN OUT PRTL_RESOURCE Resource
);

#endif // r3

NATIVE_API
USHORT
NTAPI
RtlCaptureStackBackTrace(
    IN  ULONG   FramesToSkip,
    IN  ULONG   FramesToCapture,
    OUT PVOID*  BackTrace,
    OUT PULONG  BackTraceHash OPTIONAL
);

#endif // _NTRTL_H_0fd28a55_9a32_4533_8006_a0469dd77ef2_