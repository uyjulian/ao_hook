#ifndef _NTPROCESS_H_39560d0d_86be_4220_839a_0fcdb8e556cf_
#define _NTPROCESS_H_39560d0d_86be_4220_839a_0fcdb8e556cf_




/************************************************************************
  process information
************************************************************************/

typedef struct _PROCESS_TIMES
{
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER ExitTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;

} PROCESS_TIMES, *PPROCESS_TIMES;

typedef struct
{
    UNICODE_STRING  ImageFileName;
    WCHAR           Buffer[1];
} PROCESS_IMAGE_FILE_NAME, *PPROCESS_IMAGE_FILE_NAME;

typedef struct
{
    UNICODE_STRING  ImageFileName;
    WCHAR           Buffer[MAX_NTPATH];
} PROCESS_IMAGE_FILE_NAME2;



typedef struct
{
    ULONG       Length;                             // +0x00
    BOOL        Initialized;                        // +0x04
    PVOID       SsHandle;                           // +0x08
    LIST_ENTRY  InLoadOrderModuleList;              // +0x0c
    LIST_ENTRY  InMemoryOrderModuleList;            // +0x14
    LIST_ENTRY  InInitializationOrderModuleList;    // +0x1c
    PVOID       EntryInProgress;                    // +0x24
    ULONG       ShutdownInProgress;                 // +0x28
    ULONG_PTR   ShutdownThreadId;                   // +0x2C
} PEB_LDR_DATA, *PPEB_LDR_DATA;                     // +0x30

typedef struct
{
    UNICODE_STRING  DosPath;
    HANDLE          Handle;
} CURDIR;

typedef struct _RTLP_CURDIR_REF *PRTLP_CURDIR_REF;

typedef struct _RTL_RELATIVE_NAME_U
{
    UNICODE_STRING RelativeName;
    HANDLE ContainingDirectory;
    PRTLP_CURDIR_REF CurDirRef;
} RTL_RELATIVE_NAME_U, *PRTL_RELATIVE_NAME_U;

typedef struct
{
/* 0x000 */ USHORT      Flags;
/* 0x002 */ USHORT      Length;
/* 0x004 */ ULONG       TimeStamp;
/* 0x008 */ ANSI_STRING DosPath;
} RTL_DRIVE_LETTER_CURDIR;

#define USER_PROCESS_PARAMETERS_NORMALIZED  (1u << 0)

typedef struct
{
/* 0x000 */ ULONG                   MaximumLength;
/* 0x004 */ ULONG                   Length;
/* 0x008 */ ULONG                   Flags;
/* 0x00c */ ULONG                   DebugFlags;
/* 0x010 */ HANDLE                  ConsoleHandle;
/* 0x014 */ ULONG                   ConsoleFlags;
/* 0x018 */ HANDLE                  StandardInput;
/* 0x01c */ HANDLE                  StandardOutput;
/* 0x020 */ HANDLE                  StandardError;
/* 0x024 */ CURDIR                  CurrentDirectory;
/* 0x030 */ UNICODE_STRING          DllPath;
/* 0x038 */ UNICODE_STRING          ImagePathName;
/* 0x040 */ UNICODE_STRING          CommandLine;
/* 0x048 */ PWCHAR                  Environment;
/* 0x04c */ ULONG                   StartingX;
/* 0x050 */ ULONG                   StartingY;
/* 0x054 */ ULONG                   CountX;
/* 0x058 */ ULONG                   CountY;
/* 0x05c */ ULONG                   CountCharsX;
/* 0x060 */ ULONG                   CountCharsY;
/* 0x064 */ ULONG                   FillAttribute;
/* 0x068 */ ULONG                   WindowFlags;
/* 0x06c */ ULONG                   ShowWindowFlags;
/* 0x070 */ UNICODE_STRING          WindowTitle;
/* 0x078 */ UNICODE_STRING          DesktopInfo;
/* 0x080 */ UNICODE_STRING          ShellInfo;
/* 0x088 */ UNICODE_STRING          RuntimeData;
/* 0x090 */ RTL_DRIVE_LETTER_CURDIR CurrentDirectores[32];
/* 0x290 */ ULONG_PTR               EnvironmentSize;
/* 0x294 */ ULONG_PTR               EnvironmentVersion;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

//  NtCreateUserProcess defs

typedef enum _PS_CREATE_STATE
{
    PsCreateInitialState            = 0,
    PsCreateFailOnFileOpen          = 1,
    PsCreateFailOnSectionCreate     = 2,
    PsCreateFailExeFormat           = 3,
    PsCreateFailMachineMismatch     = 4,
    PsCreateFailExeName             = 5,    // Debugger specified
    PsCreateSuccess                 = 6,

    PsCreateMaximumStates

} PS_CREATE_STATE;

typedef struct _PS_CREATE_INFO
{
    SIZE_T Size;
    PS_CREATE_STATE State;
    union
    {
        // PsCreateInitialState
        struct
        {
            union
            {
                ULONG InitFlags;
                struct
                {
                    UCHAR WriteOutputOnExit : 1;
                    UCHAR DetectManifest : 1;
                    UCHAR SpareBits1 : 6;
                    UCHAR IFEOKeyState : 2; // PS_IFEO_KEY_STATE
                    UCHAR SpareBits2 : 6;
                    USHORT ProhibitedImageCharacteristics : 16;
                };
            };
            ACCESS_MASK AdditionalFileAccess;
        } InitState;

        // PsCreateFailOnSectionCreate
        struct
        {
            HANDLE FileHandle;
        } FailSection;

        // PsCreateFailExeName
        struct
        {
            HANDLE IFEOKey;
        } ExeName;

        // PsCreateSuccess
        struct
        {
            union
            {
                ULONG OutputFlags;
                struct
                {
                    UCHAR ProtectedProcess : 1;
                    UCHAR AddressSpaceOverride : 1;
                    UCHAR DevOverrideEnabled : 1; // from Image File Execution Options
                    UCHAR ManifestDetected : 1;
                    UCHAR SpareBits1 : 4;
                    UCHAR SpareBits2 : 8;
                    USHORT SpareBits3 : 16;
                };
            };

            HANDLE FileHandle;
            HANDLE SectionHandle;
            ULONGLONG UserProcessParametersNative;
            ULONG UserProcessParametersWow64;
            ULONG CurrentParameterFlags;
            ULONGLONG PebAddressNative;
            ULONG PebAddressWow64;
            ULONGLONG ManifestAddress;
            ULONG ManifestSize;
        } SuccessState;
    };
} PS_CREATE_INFO, *PPS_CREATE_INFO;

// Attributes

// begin_rev
#define PS_ATTRIBUTE_NUMBER_MASK    0x0000FFFF
#define PS_ATTRIBUTE_THREAD         0x00010000 // can be used with threads
#define PS_ATTRIBUTE_INPUT          0x00020000 // input only
#define PS_ATTRIBUTE_UNKNOWN        0x00040000

#define PS_ATTRIBUTE_FLAG_THREAD    0x0001 // can be used with threads
#define PS_ATTRIBUTE_FLAG_INPUT     0x0002 // input only
#define PS_ATTRIBUTE_FLAG_UNKNOWN   0x0004

#define PS_ATTRIBUTE_FLAG_INPUT_HANDLE   (PS_ATTRIBUTE_FLAG_INPUT | PS_ATTRIBUTE_FLAG_UNKNOWN)

// end_rev

// private
typedef enum _PS_ATTRIBUTE_NUM
{
    PsAttributeParentProcess    = 0, // in HANDLE
    PsAttributeDebugPort        = 1, // in HANDLE
    PsAttributeToken            = 2, // in HANDLE
    PsAttributeClientId         = 3, // out PCLIENT_ID
    PsAttributeTebAddress       = 4, // out PTEB *
    PsAttributeImageName        = 5, // in PWSTR
    PsAttributeImageInfo        = 6, // out PSECTION_IMAGE_INFORMATION
    PsAttributeMemoryReserve    = 7, // in PPS_MEMORY_RESERVE
    PsAttributePriorityClass    = 8, // in UCHAR
    PsAttributeErrorMode        = 9, // in ULONG
    PsAttributeStdHandleInfo    = 10, // in PPS_STD_HANDLE_INFO
    PsAttributeHandleList       = 11, // in PHANDLE
    PsAttributeGroupAffinity    = 12, // in PGROUP_AFFINITY
    PsAttributePreferredNode    = 13, // in PUSHORT
    PsAttributeIdealProcessor   = 14, // in PPROCESSOR_NUMBER
    PsAttributeUmsThread        = 15, // ? in PUMS_CREATE_THREAD_ATTRIBUTES
    PsAttributeMitigationOptions= 16, // in UCHAR
    PsAttributeMax

} PS_ATTRIBUTE_NUM;

typedef struct _PS_STD_HANDLE_INFO
{
    union
    {
/* 0x000 */ ULONG                          Flags;

        struct
        {
/* 0x000 */     UCHAR                      StdHandleState : 2;
/* 0x000 */     UCHAR                      PseudoHandleMask : 3;
        };
    };

/* 0x004 */ ULONG                          StdHandleSubsystemType;

} PS_STD_HANDLE_INFO, *PPS_STD_HANDLE_INFO;

typedef struct _PS_MEMORY_RESERVE
{
/* 0x000 */ PVOID   ReserveAddress;
/* 0x004 */ ULONG   ReserveSize;

} PS_MEMORY_RESERVE, *PPS_MEMORY_RESERVE;

// begin_rev

#define PsAttributeValue(Number, Thread, Input, Unknown) \
    (((Number) & PS_ATTRIBUTE_NUMBER_MASK) | \
    ((Thread) ? PS_ATTRIBUTE_THREAD : 0) | \
    ((Input) ? PS_ATTRIBUTE_INPUT : 0) | \
    ((Unknown) ? PS_ATTRIBUTE_UNKNOWN : 0))

#define PS_ATTRIBUTE_PARENT_PROCESS         PsAttributeValue(PsAttributeParentProcess, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_DEBUG_PORT             PsAttributeValue(PsAttributeDebugPort, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_TOKEN                  PsAttributeValue(PsAttributeToken, FALSE, TRUE, TRUE)
#define PS_ATTRIBUTE_CLIENT_ID              PsAttributeValue(PsAttributeClientId, TRUE, FALSE, FALSE)
#define PS_ATTRIBUTE_TEB_ADDRESS            PsAttributeValue(PsAttributeTebAddress, TRUE, FALSE, FALSE)
#define PS_ATTRIBUTE_IMAGE_NAME             PsAttributeValue(PsAttributeImageName, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_IMAGE_INFO             PsAttributeValue(PsAttributeImageInfo, FALSE, FALSE, FALSE)
#define PS_ATTRIBUTE_MEMORY_RESERVE         PsAttributeValue(PsAttributeMemoryReserve, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_PRIORITY_CLASS         PsAttributeValue(PsAttributePriorityClass, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_ERROR_MODE             PsAttributeValue(PsAttributeErrorMode, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_STD_HANDLE_INFO        PsAttributeValue(PsAttributeStdHandleInfo, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_HANDLE_LIST            PsAttributeValue(PsAttributeHandleList, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_GROUP_AFFINITY         PsAttributeValue(PsAttributeGroupAffinity, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_PREFERRED_NODE         PsAttributeValue(PsAttributePreferredNode, FALSE, TRUE, FALSE)
#define PS_ATTRIBUTE_IDEAL_PROCESSOR        PsAttributeValue(PsAttributeIdealProcessor, TRUE, TRUE, FALSE)
#define PS_ATTRIBUTE_MITIGATION_OPTIONS     PsAttributeValue(PsAttributeMitigationOptions, FALSE, TRUE, TRUE)

// end_rev

// begin_private

typedef struct _PS_ATTRIBUTE
{
//    ULONG       Attribute;
    USHORT      AttributeNumber;
    USHORT      AttributeFlags;

    ULONG_PTR   Size;

    union
    {
        ULONG_PTR   Value;
        PVOID       ValuePtr;
    };

    PULONG_PTR  ReturnLength;

} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST
{
    ULONG_PTR       TotalLength;
    PS_ATTRIBUTE    Attributes[1];
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;

#define PROCESS_CREATE_FLAGS_BREAKAWAY              0x00000001
#define PROCESS_CREATE_FLAGS_NO_DEBUG_INHERIT       0x00000002
#define PROCESS_CREATE_FLAGS_INHERIT_HANDLES        0x00000004
#define PROCESS_CREATE_FLAGS_OVERRIDE_ADDRESS_SPACE 0x00000008
#define PROCESS_CREATE_FLAGS_LARGE_PAGES            0x00000010

#define THREAD_CREATE_FLAGS_CREATE_SUSPENDED        0x00000001
#define THREAD_CREATE_FLAGS_SKIP_THREAD_ATTACH      0x00000002 // ?
#define THREAD_CREATE_FLAGS_HIDE_FROM_DEBUGGER      0x00000004
#define THREAD_CREATE_FLAGS_HAS_SECURITY_DESCRIPTOR 0x00000010 // ?
#define THREAD_CREATE_FLAGS_ACCESS_CHECK_IN_TARGET  0x00000020 // ?
#define THREAD_CREATE_FLAGS_INITIAL_THREAD          0x00000080

#if CPP_DEFINED

IF_NOT_EXIST(EXCEPTION_ROUTINE)
{
    typedef
    EXCEPTION_DISPOSITION
    STDCALL
    EXCEPTION_ROUTINE(
        PEXCEPTION_RECORD   ExceptionRecord,
        PVOID               EstablisherFrame,
        PCONTEXT            ContextRecord,
        PVOID               DispatcherContext
    );
}

#endif // CPP_DEFINED

#ifndef _WIN32_WINNT_WIN8

typedef struct _EXCEPTION_REGISTRATION_RECORD
{
/* 0x000 */ struct _EXCEPTION_REGISTRATION_RECORD   *Next;
/* 0x004 */ EXCEPTION_ROUTINE                       *Handler;
} EXCEPTION_REGISTRATION_RECORD, *PEXCEPTION_REGISTRATION_RECORD;

#endif // _WIN32_WINNT_WIN8

#if 0
typedef struct _NT_TIB
{
/* 0x000 */ EXCEPTION_REGISTRATION_RECORD *ExceptionList;
/* 0x004 */ PVOID StackBase;
/* 0x008 */ PVOID StackLimit;
/* 0x00c */ PVOID SubSystemTib;
/* 0x010 */ union
            {
                PVOID FiberData;
                ULONG Version;
            };
/* 0x014 */ PVOID ArbitraryUserPointer;
/* 0x018 */ struct _NT_TIB *Self;
} NT_TIB;

#endif

typedef struct
{
    UNICODE_STRING  WindowsDirectory;
    UNICODE_STRING  SystemDirectory;
    UNICODE_STRING  BaseNamedObjects;
    ULONG           Unknown;
    USHORT          Unknown2;
    USHORT          ServicePackLength;
    USHORT          ServicePackMaximumLength;
    WCHAR           ServicePack[1];

} *PSTATIC_SERVER_DATA;

typedef struct
{
    UNICODE_STRING64    WindowsDirectory;
    UNICODE_STRING64    SystemDirectory;
    UNICODE_STRING64    BaseNamedObjects;
    ULONG               Unknown;
    USHORT              Unknown2;
    USHORT              ServicePackLength;
    USHORT              ServicePackMaximumLength;
    WCHAR               ServicePack[1];

} *PSTATIC_SERVER_DATA64;

typedef struct
{
    PVOID               Reserve;
    PSTATIC_SERVER_DATA StaticServerData;

} *STATIC_SERVER_DATA_PTR;

//
// Global flags that can be set to control system behavior.
// Flag word is 32 bits.
//

#define FLG_STOP_ON_EXCEPTION           0x00000001      // user and kernel mode
#define FLG_SHOW_LDR_SNAPS              0x00000002      // user and kernel mode
#define FLG_DEBUG_INITIAL_COMMAND       0x00000004      // kernel mode only up until WINLOGON started
#define FLG_STOP_ON_HUNG_GUI            0x00000008      // kernel mode only while running

#define FLG_HEAP_ENABLE_TAIL_CHECK      0x00000010      // user mode only
#define FLG_HEAP_ENABLE_FREE_CHECK      0x00000020      // user mode only
#define FLG_HEAP_VALIDATE_PARAMETERS    0x00000040      // user mode only
#define FLG_HEAP_VALIDATE_ALL           0x00000080      // user mode only

#define FLG_APPLICATION_VERIFIER        0x00000100      // user mode only
#define FLG_POOL_ENABLE_TAGGING         0x00000400      // kernel mode only
#define FLG_HEAP_ENABLE_TAGGING         0x00000800      // user mode only

#define FLG_USER_STACK_TRACE_DB         0x00001000      // x86 user mode only
#define FLG_KERNEL_STACK_TRACE_DB       0x00002000      // x86 kernel mode only at boot time
#define FLG_MAINTAIN_OBJECT_TYPELIST    0x00004000      // kernel mode only at boot time
#define FLG_HEAP_ENABLE_TAG_BY_DLL      0x00008000      // user mode only

#define FLG_DISABLE_STACK_EXTENSION     0x00010000      // user mode only
#define FLG_ENABLE_CSRDEBUG             0x00020000      // kernel mode only at boot time
#define FLG_ENABLE_KDEBUG_SYMBOL_LOAD   0x00040000      // kernel mode only
#define FLG_DISABLE_PAGE_KERNEL_STACKS  0x00080000      // kernel mode only at boot time

#define FLG_ENABLE_SYSTEM_CRIT_BREAKS   0x00100000      // user mode only
#define FLG_HEAP_DISABLE_COALESCING     0x00200000      // user mode only
#define FLG_ENABLE_CLOSE_EXCEPTIONS     0x00400000      // kernel mode only
#define FLG_ENABLE_EXCEPTION_LOGGING    0x00800000      // kernel mode only

#define FLG_ENABLE_HANDLE_TYPE_TAGGING  0x01000000      // kernel mode only
#define FLG_HEAP_PAGE_ALLOCS            0x02000000      // user mode only
#define FLG_DEBUG_INITIAL_COMMAND_EX    0x04000000      // kernel mode only up until WINLOGON started
#define FLG_DISABLE_DBGPRINT            0x08000000      // kernel mode only

#define FLG_CRITSEC_EVENT_CREATION      0x10000000      // user mode only, Force early creation of resource events
#define FLG_LDR_TOP_DOWN                0x20000000      // user mode only, win64 only
#define FLG_ENABLE_HANDLE_EXCEPTIONS    0x40000000      // kernel mode only
#define FLG_DISABLE_PROTDLLS            0x80000000      // user mode only (smss/winlogon)

#define FLG_VALID_BITS                  0xFFFFFDFF

#define FLG_USERMODE_VALID_BITS        (FLG_STOP_ON_EXCEPTION           | \
                                        FLG_SHOW_LDR_SNAPS              | \
                                        FLG_HEAP_ENABLE_TAIL_CHECK      | \
                                        FLG_HEAP_ENABLE_FREE_CHECK      | \
                                        FLG_HEAP_VALIDATE_PARAMETERS    | \
                                        FLG_HEAP_VALIDATE_ALL           | \
                                        FLG_APPLICATION_VERIFIER        | \
                                        FLG_HEAP_ENABLE_TAGGING         | \
                                        FLG_USER_STACK_TRACE_DB         | \
                                        FLG_HEAP_ENABLE_TAG_BY_DLL      | \
                                        FLG_DISABLE_STACK_EXTENSION     | \
                                        FLG_ENABLE_SYSTEM_CRIT_BREAKS   | \
                                        FLG_HEAP_DISABLE_COALESCING     | \
                                        FLG_DISABLE_PROTDLLS            | \
                                        FLG_HEAP_PAGE_ALLOCS            | \
                                        FLG_CRITSEC_EVENT_CREATION      | \
                                        FLG_LDR_TOP_DOWN)

#define FLG_BOOTONLY_VALID_BITS        (FLG_KERNEL_STACK_TRACE_DB       | \
                                        FLG_MAINTAIN_OBJECT_TYPELIST    | \
                                        FLG_ENABLE_CSRDEBUG             | \
                                        FLG_DEBUG_INITIAL_COMMAND       | \
                                        FLG_DEBUG_INITIAL_COMMAND_EX    | \
                                        FLG_DISABLE_PAGE_KERNEL_STACKS)

#define FLG_KERNELMODE_VALID_BITS      (FLG_STOP_ON_EXCEPTION           | \
                                        FLG_SHOW_LDR_SNAPS              | \
                                        FLG_STOP_ON_HUNG_GUI            | \
                                        FLG_POOL_ENABLE_TAGGING         | \
                                        FLG_ENABLE_KDEBUG_SYMBOL_LOAD   | \
                                        FLG_ENABLE_CLOSE_EXCEPTIONS     | \
                                        FLG_ENABLE_EXCEPTION_LOGGING    | \
                                        FLG_ENABLE_HANDLE_TYPE_TAGGING  | \
                                        FLG_DISABLE_DBGPRINT            | \
                                        FLG_ENABLE_HANDLE_EXCEPTIONS      \
                                       )

typedef struct PEB_BASE
{
/* 0x000 */ UCHAR                           InheritedAddressSpace;
/* 0x001 */ UCHAR                           ReadImageFileExecOptions;
/* 0x002 */ UCHAR                           BeingDebugged;
/* 0x003 */ struct
            {
                UCHAR                       ImageUsesLargePages             : 1;
                UCHAR                       IsProtectedProcess              : 1;
                UCHAR                       IsLegacyProcess                 : 1;
                UCHAR                       IsImageDynamicallyRelocated     : 1;
                UCHAR                       SkipPatchingUser32Forwarders    : 1;
                UCHAR                       SpareBits                       : 3;
            };
/* 0x004 */ PVOID                           Mutant;
/* 0x008 */ PVOID                           ImageBaseAddress;
/* 0x00c */ PPEB_LDR_DATA                   Ldr;
/* 0x010 */ PRTL_USER_PROCESS_PARAMETERS    ProcessParameters;
/* 0x014 */ PVOID                           SubSystemData;
/* 0x018 */ HANDLE                          ProcessHeap;
/* 0x01c */ PRTL_CRITICAL_SECTION           FastPebLock;
/* 0x020 */ PVOID                           AtlThunkSListPtr;
/* 0x024 */ PVOID                           IFEOKey;
/* 0x028 */ union
            {
                ULONG_PTR CrossProcessFlags;
                struct
                {
                    UCHAR ProcessInJob           : 1;
                    UCHAR ProcessInitializing    : 1;
                    UCHAR ProcessUsingVEH        : 1;
                    UCHAR ProcessUsingVCH        : 1;
                    UCHAR ProcessUsingFTH        : 1;
                };
            };

/* 0x02C */ union
            {
                PVOID                       KernelCallbackTable;
                PVOID                       UserSharedInfoPtr;
            };

/* 0x030 */ ULONG                           SystemReserved[1];
/* 0x034 */ ULONG                           AtlThunkSListPtr32;
/* 0x038 */ PVOID                           ApiSetMap;
/* 0x03c */ ULONG                           TlsExpansionCounter;
/* 0x040 */ PVOID                           TlsBitmap;
/* 0x044 */ ULONG                           TlsBitmapBits[2];
/* 0x04c */ PVOID                           ReadOnlySharedMemoryBase;
/* 0x050 */ PVOID                           HotpatchInformation;
/* 0x054 */ STATIC_SERVER_DATA_PTR          ReadOnlyStaticServerData;
/* 0x058 */ PUSHORT                         AnsiCodePageData;
/* 0x05c */ PUSHORT                         OemCodePageData;
/* 0x060 */ PVOID                           UnicodeCaseTableData;
/* 0x064 */ ULONG                           NumberOfProcessors;
/* 0x068 */ ULONG                           NtGlobalFlag;
/* 0x06C */ ULONG                           Dummy;
/* 0x070 */ LARGE_INTEGER                   CriticalSectionTimeout;
/* 0x078 */ ULONG                           HeapSegmentReserve;
/* 0x07c */ ULONG                           HeapSegmentCommit;
/* 0x080 */ ULONG                           HeapDeCommitTotalFreeThreshold;
/* 0x084 */ ULONG                           HeapDeCommitFreeBlockThreshold;
/* 0x088 */ ULONG                           NumberOfHeaps;
/* 0x08c */ ULONG                           MaximumNumberOfHeaps;
/* 0x090 */ PVOID                           ProcessHeaps;
/* 0x094 */ PVOID                           GdiSharedHandleTable;
/* 0x098 */ PVOID                           ProcessStarterHelper;
/* 0x09c */ ULONG                           GdiDCAttributeList;
/* 0x0a0 */ PRTL_CRITICAL_SECTION           LoaderLock;
/* 0x0a4 */ ULONG                           OSMajorVersion;
/* 0x0a8 */ ULONG                           OSMinorVersion;
/* 0x0ac */ USHORT                          OSBuildNumber;
/* 0x0ae */ USHORT                          OSCSDVersion;
/* 0x0b0 */ ULONG                           OSPlatformId;
} PEB_BASE, *PPEB_BASE;

typedef struct TEB_BASE
{
/* 0x000 */ NT_TIB      NtTib;
/* 0x01C */ PVOID       EnvironmentPointer;
/* 0x020 */ CLIENT_ID   ClientId;
/* 0x028 */ HANDLE      ActiveRpcHandle;
/* 0x02C */ PVOID       ThreadLocalStoragePointer;
/* 0x030 */ PPEB_BASE   ProcessEnvironmentBlock;
/* 0x034 */ ULONG       LastErrorValue;
/* 0x038 */ ULONG       CountOfOwnedCriticalSections;
/* 0x03C */ PVOID       CsrClientThread;
/* 0x040 */ PVOID       Win32ThreadInfo;
/* 0x044 */ ULONG       User32Reserved[26];
/* 0x0AC */ ULONG       UserReserved[5];
/* 0x0C0 */ PVOID       WOW32Reserved;
/* 0x0C4 */ ULONG       CurrentLocale;
/* 0x0C8 */ ULONG       FpSoftwareStatusRegister;
/* 0x0CC */ PVOID       SystemReserved1[54];
/* 0x1A4 */ LONG        ExceptionCode;

} TEB_BASE, *PTEB_BASE;

/************************************************************************
  thread information
************************************************************************/

#ifndef _LDT_ENTRY_DEFINED
#define _LDT_ENTRY_DEFINED

typedef struct _LDT_ENTRY {
    WORD    LimitLow;
    WORD    BaseLow;
    union {
        struct {
            BYTE    BaseMid;
            BYTE    Flags1;     // Declare as bytes to avoid alignment
            BYTE    Flags2;     // Problems.
            BYTE    BaseHi;
        } Bytes;
        struct {
            DWORD   BaseMid : 8;
            DWORD   Type : 5;
            DWORD   Dpl : 2;
            DWORD   Pres : 1;
            DWORD   LimitHi : 4;
            DWORD   Sys : 1;
            DWORD   Reserved_0 : 1;
            DWORD   Default_Big : 1;
            DWORD   Granularity : 1;
            DWORD   BaseHi : 8;
        } Bits;
    } HighWord;
} LDT_ENTRY, *PLDT_ENTRY;

#endif

typedef struct _DESCRIPTOR_TABLE_ENTRY {
    ULONG Selector;
    LDT_ENTRY Descriptor;
} DESCRIPTOR_TABLE_ENTRY, *PDESCRIPTOR_TABLE_ENTRY;

typedef
VOID
(NTAPI
*PKNORMAL_ROUTINE)(
    IN PVOID NormalContext OPTIONAL,
    IN PVOID SystemArgument1 OPTIONAL,
    IN PVOID SystemArgument2 OPTIONAL
);

#if ML_KERNEL_MODE


#define PROCESS_TERMINATE                  (0x0001)
#define PROCESS_CREATE_THREAD              (0x0002)
#define PROCESS_SET_SESSIONID              (0x0004)
#define PROCESS_VM_OPERATION               (0x0008)
#define PROCESS_VM_READ                    (0x0010)
#define PROCESS_VM_WRITE                   (0x0020)
#define PROCESS_DUP_HANDLE                 (0x0040)
#define PROCESS_CREATE_PROCESS             (0x0080)
#define PROCESS_SET_QUOTA                  (0x0100)
#define PROCESS_SET_INFORMATION            (0x0200)
#define PROCESS_QUERY_INFORMATION          (0x0400)
#define PROCESS_SUSPEND_RESUME             (0x0800)
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)
#define PROCESS_SET_LIMITED_INFORMATION    (0x2000)
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFFF)
#else
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFF)
#endif

typedef
VOID
(NTAPI
*PKKERNEL_ROUTINE)(
    PKAPC               Apc,
    PKNORMAL_ROUTINE*   NormalRoutine,
    PVOID*              NormalContext,
    PVOID*              SystemArgument1,
    PVOID*              SystemArgument2
);

typedef
VOID
(NTAPI
*PKRUNDOWN_ROUTINE)(
    PKAPC *Apc
);

#endif // r0

#if ML_USER_MODE

//
// Define process debug flags
//
#define PROCESS_DEBUG_INHERIT 0x00000001

typedef enum
{
    ProcessBasicInformation                     = 0,        // 0, q: PROCESS_BASIC_INFORMATION, PROCESS_EXTENDED_BASIC_INFORMATION
    ProcessQuotaLimits                          = 1,        // qs: QUOTA_LIMITS, QUOTA_LIMITS_EX
    ProcessIoCounters                           = 2,        // q: IO_COUNTERS
    ProcessVmCounters                           = 3,        // q: VM_COUNTERS, VM_COUNTERS_EX
    ProcessTimes                                = 4,        // q: KERNEL_USER_TIMES
    ProcessBasePriority                         = 5,        // s: KPRIORITY
    ProcessRaisePriority                        = 6,        // s: ULONG
    ProcessDebugPort                            = 7,        // q: HANDLE
    ProcessExceptionPort                        = 8,        // s: HANDLE
    ProcessAccessToken                          = 9,        // s: PROCESS_ACCESS_TOKEN
    ProcessLdtInformation                       = 10,
    ProcessLdtSize                              = 11,
    ProcessDefaultHardErrorMode                 = 12,       // qs: ULONG
    ProcessIoPortHandlers                       = 13,       // (kernel-mode only)
    ProcessPooledUsageAndLimits                 = 14,       // q: POOLED_USAGE_AND_LIMITS
    ProcessWorkingSetWatch                      = 15,       // q: PROCESS_WS_WATCH_INFORMATION[]; s: void
    ProcessUserModeIOPL                         = 16,
    ProcessEnableAlignmentFaultFixup            = 17,       // s: BOOLEAN
    ProcessPriorityClass                        = 18,       // qs: PROCESS_PRIORITY_CLASS
    ProcessWx86Information                      = 19,
    ProcessHandleCount                          = 20,       // q: ULONG, PROCESS_HANDLE_INFORMATION
    ProcessAffinityMask                         = 21,       // s: KAFFINITY
    ProcessPriorityBoost                        = 22,       // qs: ULONG
    ProcessDeviceMap                            = 23,       // qs: PROCESS_DEVICEMAP_INFORMATION, PROCESS_DEVICEMAP_INFORMATION_EX
    ProcessSessionInformation                   = 24,       // q: PROCESS_SESSION_INFORMATION
    ProcessForegroundInformation                = 25,       // s: PROCESS_FOREGROUND_BACKGROUND
    ProcessWow64Information                     = 26,       // q: ULONG_PTR
    ProcessImageFileName                        = 27,       // q: UNICODE_STRING
    ProcessLUIDDeviceMapsEnabled                = 28,       // q: ULONG
    ProcessBreakOnTermination                   = 29,       // qs: ULONG
    ProcessDebugObjectHandle                    = 30,       // q: HANDLE
    ProcessDebugFlags                           = 31,       // qs: ULONG
    ProcessHandleTracing                        = 32,       // q: PROCESS_HANDLE_TRACING_QUERY; s: size 0 disables, otherwise enables
    ProcessIoPriority                           = 33,       // qs: ULONG
    ProcessExecuteFlags                         = 34,       // qs: ULONG
    ProcessTlsInformation                       = 35,
    ProcessCookie                               = 36,       // q: ULONG
    ProcessImageInformation                     = 37,       // q: SECTION_IMAGE_INFORMATION
    ProcessCycleTime                            = 38,       // q: PROCESS_CYCLE_TIME_INFORMATION
    ProcessPagePriority                         = 39,       // q: ULONG
    ProcessInstrumentationCallback              = 40,
    ProcessThreadStackAllocation                = 41,       // s: PROCESS_STACK_ALLOCATION_INFORMATION, PROCESS_STACK_ALLOCATION_INFORMATION_EX
    ProcessWorkingSetWatchEx                    = 42,       // q: PROCESS_WS_WATCH_INFORMATION_EX[]
    ProcessImageFileNameWin32                   = 43,       // q: UNICODE_STRING
    ProcessImageFileMapping                     = 44,       // q: HANDLE (input)
    ProcessAffinityUpdateMode                   = 45,       // qs: PROCESS_AFFINITY_UPDATE_MODE
    ProcessMemoryAllocationMode                 = 46,       // qs: PROCESS_MEMORY_ALLOCATION_MODE
    ProcessGroupInformation                     = 47,       // q: USHORT[]
    ProcessTokenVirtualizationEnabled           = 48,       // s: ULONG
    ProcessConsoleHostProcess                   = 49,       // q: ULONG_PTR
    ProcessWindowInformation                    = 50,       // q: PROCESS_WINDOW_INFORMATION
    ProcessHandleInformation                    = 51,       // q: PROCESS_HANDLE_SNAPSHOT_INFORMATION // since WIN8
    ProcessMitigationPolicy                     = 52,       // s: PROCESS_MITIGATION_POLICY_INFORMATION
    ProcessDynamicFunctionTableInformation      = 53,
    ProcessHandleCheckingMode                   = 54,
    ProcessKeepAliveCount                       = 55,       // q: PROCESS_KEEPALIVE_COUNT_INFORMATION
    ProcessRevokeFileHandles                    = 56,       // s: PROCESS_REVOKE_FILE_HANDLES_INFORMATION
    ProcessWorkingSetControl                    = 57,

    MaxProcessInfoClass

} PROCESSINFOCLASS;

typedef struct _PROCESS_BASIC_INFORMATION
{
    NTSTATUS    ExitStatus;
    struct PEB* PebBaseAddress;
    ULONG_PTR   AffinityMask;
    LONG        BasePriority;
    ULONG_PTR   UniqueProcessId;
    ULONG_PTR   InheritedFromUniqueProcessId;

} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

typedef struct _PROCESS_SESSION_INFORMATION {
    ULONG SessionId;
} PROCESS_SESSION_INFORMATION, *PPROCESS_SESSION_INFORMATION;


typedef struct _PROCESS_ACCESS_TOKEN {

    //
    // Handle to Primary token to assign to the process.
    // TOKEN_ASSIGN_PRIMARY access to this token is needed.
    //

    HANDLE Token;

    //
    // Handle to the initial thread of the process.
    // A process's access token can only be changed if the process has
    // no threads or a single thread that has not yet begun execution.
    //
    // N.B. This field is unused.
    //

    HANDLE Thread;

} PROCESS_ACCESS_TOKEN, *PPROCESS_ACCESS_TOKEN;

typedef struct _PROCESS_DEVICEMAP_INFORMATION {
    union {
        struct {
            HANDLE DirectoryHandle;
        } Set;
        struct {
            ULONG DriveMap;
            UCHAR DriveType[ 32 ];
        } Query;
    } DUMMYUNIONNAME;
} PROCESS_DEVICEMAP_INFORMATION, *PPROCESS_DEVICEMAP_INFORMATION;

typedef struct _PROCESS_DEVICEMAP_INFORMATION_EX {
    union {
        struct {
            HANDLE DirectoryHandle;
        } Set;
        struct {
            ULONG DriveMap;
            UCHAR DriveType[ 32 ];
        } Query;
    } DUMMYUNIONNAME;
    ULONG Flags;    // specifies that the query type
} PROCESS_DEVICEMAP_INFORMATION_EX, *PPROCESS_DEVICEMAP_INFORMATION_EX;


typedef enum
{
    ThreadBasicInformation              = 0,        // q: THREAD_BASIC_INFORMATION
    ThreadTimes                         = 1,        // q: KERNEL_USER_TIMES
    ThreadPriority                      = 2,        // s: KPRIORITY
    ThreadBasePriority                  = 3,        // s: LONG
    ThreadAffinityMask                  = 4,        // s: KAFFINITY
    ThreadImpersonationToken            = 5,        // s: HANDLE
    ThreadDescriptorTableEntry          = 6,
    ThreadEnableAlignmentFaultFixup     = 7,        // s: BOOLEAN
    ThreadEventPair_Reusable            = 8,
    ThreadQuerySetWin32StartAddress     = 9,        // q: PVOID
    ThreadZeroTlsCell                   = 10,
    ThreadPerformanceCount              = 11,       // q: LARGE_INTEGER
    ThreadAmILastThread                 = 12,       // q: ULONG
    ThreadIdealProcessor                = 13,       // s: ULONG
    ThreadPriorityBoost                 = 14,       // qs: ULONG
    ThreadSetTlsArrayAddress            = 15,       // Obsolet
    ThreadIsIoPending                   = 16,       // q: ULONG
    ThreadHideFromDebugger              = 17,       // s: void
    ThreadBreakOnTermination            = 18,       // qs: ULONG
    ThreadSwitchLegacyState             = 19,
    ThreadIsTerminated                  = 20,       // q: ULONG
    ThreadLastSystemCall                = 21,       // q: THREAD_LAST_SYSCALL_INFORMATION
    ThreadIoPriority                    = 22,       // qs: ULONG
    ThreadCycleTime                     = 23,       // q: THREAD_CYCLE_TIME_INFORMATION
    ThreadPagePriority                  = 24,       // q: ULONG
    ThreadActualBasePriority            = 25,
    ThreadTebInformation                = 26,       // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
    ThreadCSwitchMon                    = 27,       // Obsolete
    ThreadCSwitchPmu                    = 28,
    ThreadWow64Context                  = 29,       // q: WOW64_CONTEXT
    ThreadGroupInformation              = 30,       // q: GROUP_AFFINITY
    ThreadUmsInformation                = 31,
    ThreadCounterProfiling              = 32,
    ThreadIdealProcessorEx              = 33,       // q: PROCESSOR_NUMBER
    ThreadCpuAccountingInformation      = 34,       // since WIN8
    ThreadSwitchStackCheck              = 35,

    MaxThreadInfoClass

} THREADINFOCLASS;


#define LOW_PRIORITY            0              // Lowest thread priority level
#define LOW_REALTIME_PRIORITY   16    // Lowest realtime priority level
#define HIGH_PRIORITY           31            // Highest thread priority level
#define MAXIMUM_PRIORITY        32         // Number of thread priority levels

typedef LONG KPRIORITY;

typedef struct _KERNEL_USER_TIMES {
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER ExitTime;
    LARGE_INTEGER KernelTime;
    LARGE_INTEGER UserTime;
} KERNEL_USER_TIMES, *PKERNEL_USER_TIMES;


#endif // r3 only



/************************************************************************
  threads
************************************************************************/

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS    ExitStatus;
    PVOID       TebBaseAddress;
    CLIENT_ID   ClientId;
    KAFFINITY   AffinityMask;
    KPRIORITY   Priority;
    KPRIORITY   BasePriority;

} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct _THREAD_TEB_INFORMATION
{
    PVOID TebInformation; // buffer to place data in
    ULONG TebOffset; // offset in TEB to begin reading from
    ULONG BytesToRead; // number of bytes to read
} THREAD_TEB_INFORMATION, *PTHREAD_TEB_INFORMATION;


typedef struct _USER_STACK
{
    PVOID  FixedStackBase;
    PVOID  FixedStackLimit;
    PVOID  ExpandableStackBase;
    PVOID  ExpandableStackLimit;
    PVOID  ExpandableStackBottom;

} USER_STACK, *PUSER_STACK;

/************************************************************************
  Process api
************************************************************************/

/*
typedef struct _SECTION_IMAGE_INFORMATION
{
    PVOID   EntryPoint;
    ULONG   StackZeroBits;
    ULONG   StackReserved;
    ULONG   StackCommit;
    ULONG   ImageSubsystem;
    WORD    SubSystemVersionLow;
    WORD    SubSystemVersionHigh;
    ULONG   Unknown1;
    ULONG   ImageCharacteristics;
    ULONG   ImageMachineType;
    ULONG   Unknown2[3];
} SECTION_IMAGE_INFORMATION, *PSECTION_IMAGE_INFORMATION;
*/
typedef struct _RTL_USER_PROCESS_INFORMATION
{
    ULONG                       Size;
    HANDLE                      ProcessHandle;
    HANDLE                      ThreadHandle;
    CLIENT_ID                   ClientId;
    SECTION_IMAGE_INFORMATION   ImageInformation;
} RTL_USER_PROCESS_INFORMATION, *PRTL_USER_PROCESS_INFORMATION;


typedef struct _INITIAL_TEB
{
    struct
    {
        PVOID OldStackBase;
        PVOID OldStackLimit;
    } OldInitialTeb;

    PVOID StackBase;
    PVOID StackLimit;
    PVOID StackAllocationBase;

} INITIAL_TEB, *PINITIAL_TEB;

typedef struct _WOW64_PROCESS
{
    PVOID Wow64;
} WOW64_PROCESS, *PWOW64_PROCESS;


NATIVE_API
HANDLE
NTAPI
CsrGetProcessId(
    VOID
);


#define RTL_USER_PROC_PARAMS_NORMALIZED         0x00000001
#define RTL_USER_PROC_PROFILE_USER              0x00000002
#define RTL_USER_PROC_PROFILE_KERNEL            0x00000004
#define RTL_USER_PROC_PROFILE_SERVER            0x00000008
#define RTL_USER_PROC_RESERVE_1MB               0x00000020
#define RTL_USER_PROC_RESERVE_16MB              0x00000040
#define RTL_USER_PROC_CASE_SENSITIVE            0x00000080
#define RTL_USER_PROC_DISABLE_HEAP_DECOMMIT     0x00000100
#define RTL_USER_PROC_DLL_REDIRECTION_LOCAL     0x00001000
#define RTL_USER_PROC_APP_MANIFEST_PRESENT      0x00002000
#define RTL_USER_PROC_IMAGE_KEY_MISSING         0x00004000
#define RTL_USER_PROC_OPTIN_PROCESS             0x00020000

NATIVE_API
NTSTATUS
NTAPI
RtlCreateProcessParameters(
    OUT PRTL_USER_PROCESS_PARAMETERS*   ProcessParameters,
    IN  PUNICODE_STRING                 ImagePathName,
    IN  PUNICODE_STRING                 DllPath             OPTIONAL,
    IN  PUNICODE_STRING                 CurrentDirectory    OPTIONAL,
    IN  PUNICODE_STRING                 CommandLine         OPTIONAL,
    IN  PWCHAR                          Environment         OPTIONAL,
    IN  PUNICODE_STRING                 WindowTitle         OPTIONAL,
    IN  PUNICODE_STRING                 DesktopInfo         OPTIONAL,
    IN  PUNICODE_STRING                 ShellInfo           OPTIONAL,
    IN  PUNICODE_STRING                 RuntimeData         OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
RtlCreateProcessParametersEx(
    OUT PRTL_USER_PROCESS_PARAMETERS*   ProcessParameters,
    IN  PUNICODE_STRING                 ImagePathName,
    IN  PUNICODE_STRING                 DllPath          OPTIONAL,
    IN  PUNICODE_STRING                 CurrentDirectory OPTIONAL,
    IN  PUNICODE_STRING                 CommandLine      OPTIONAL,
    IN  PVOID                           Environment      OPTIONAL,
    IN  PUNICODE_STRING                 WindowTitle      OPTIONAL,
    IN  PUNICODE_STRING                 DesktopInfo      OPTIONAL,
    IN  PUNICODE_STRING                 ShellInfo        OPTIONAL,
    IN  PUNICODE_STRING                 RuntimeData      OPTIONAL,
    IN  ULONG                           Flags // pass RTL_USER_PROC_PARAMS_NORMALIZED to keep parameters normalized
);

NATIVE_API
NTSTATUS
NTAPI
RtlDestroyProcessParameters(
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters
);

NATIVE_API
PRTL_USER_PROCESS_PARAMETERS
NTAPI
RtlNormalizeProcessParams(
    IN OUT PRTL_USER_PROCESS_PARAMETERS ProcessParameters
);

NATIVE_API
PRTL_USER_PROCESS_PARAMETERS
NTAPI
RtlDeNormalizeProcessParams(
    IN OUT PRTL_USER_PROCESS_PARAMETERS ProcessParameters
);

NATIVE_API
NTSTATUS
NTAPI
RtlCreateUserProcess(
    IN      PUNICODE_STRING                 ImagePath,
    IN      ULONG                           ObjectAttributes,
    IN OUT  PRTL_USER_PROCESS_PARAMETERS    ProcessParameters,
    IN      PSECURITY_DESCRIPTOR            ProcessSecurityDescriptor OPTIONAL,
    IN      PSECURITY_DESCRIPTOR            ThreadSecurityDescriptor OPTIONAL,
    IN      HANDLE                          ParentProcess,
    IN      BOOLEAN                         InheritHandles,
    IN      HANDLE                          DebugPort OPTIONAL,
    IN      HANDLE                          ExceptionPort OPTIONAL,
    OUT     PRTL_USER_PROCESS_INFORMATION   ProcessInformation
);

NATIVE_API
NTSTATUS
NTAPI
RtlQueryEnvironmentVariable_U(
    IN PVOID                Environment OPTIONAL,
    IN PUNICODE_STRING      VariableName,
    OUT PUNICODE_STRING     VariableValue
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryInformationProcess(
    HANDLE              ProcessHandle,
    PROCESSINFOCLASS    ProcessInformationClass,
    PVOID               ProcessInformation,
    ULONG               ProcessInformationLength,
    PULONG              ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryInformationProcess(
    HANDLE                      ProcessHandle,
    PROCESSINFOCLASS   ProcessInformationClass,
    PVOID                       ProcessInformation,
    ULONG                       ProcessInformationLength,
    PULONG                      ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
NtSetInformationProcess(
    IN HANDLE                       ProcessHandle,
    IN PROCESSINFOCLASS    ProcessInformationClass,
    IN PVOID                        ProcessInformation,
    IN ULONG                        ProcessInformationLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetInformationProcess(
    IN HANDLE                       ProcessHandle,
    IN PROCESSINFOCLASS    ProcessInformationClass,
    IN PVOID                        ProcessInformation,
    IN ULONG                        ProcessInformationLength
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateProcess(
    OUT PHANDLE             ProcessHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  HANDLE              ParentProcess,
    IN  BOOLEAN             InheritObjectTable,
    IN  HANDLE              SectionHandle OPTIONAL,
    IN  HANDLE              DebugPort OPTIONAL,
    IN  HANDLE              ExceptionPort OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateProcessEx(
    OUT PHANDLE             ProcessHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  HANDLE              ParentProcess,
    IN  ULONG               Flags,
    IN  HANDLE              SectionHandle OPTIONAL,
    IN  HANDLE              DebugPort OPTIONAL,
    IN  HANDLE              ExceptionPort OPTIONAL,
    IN  ULONG               JobMemberLevel
);


typedef struct
{
    PHANDLE                         ProcessHandle;
    PHANDLE                         ThreadHandle;
    ACCESS_MASK                     ProcessDesiredAccess;
    ACCESS_MASK                     ThreadDesiredAccess;
    POBJECT_ATTRIBUTES              ProcessObjectAttributes;
    POBJECT_ATTRIBUTES              ThreadObjectAttributes;
    ULONG                           ProcessFlags;
    ULONG                           ThreadFlags;
    PRTL_USER_PROCESS_PARAMETERS    ProcessParameters;
    PPS_CREATE_INFO                 CreateInfo;
    PPS_ATTRIBUTE_LIST              AttributeList;
    NTSTATUS                        Status;

} CREATE_USER_PROCESS_PARAMETER, *PCREATE_USER_PROCESS_PARAMETER;


NATIVE_API
NTSTATUS
NTAPI
NtCreateUserProcess(
    OUT     PHANDLE                         ProcessHandle,
    OUT     PHANDLE                         ThreadHandle,
    IN      ACCESS_MASK                     ProcessDesiredAccess,
    IN      ACCESS_MASK                     ThreadDesiredAccess,
    IN      POBJECT_ATTRIBUTES              ProcessObjectAttributes OPTIONAL,
    IN      POBJECT_ATTRIBUTES              ThreadObjectAttributes OPTIONAL,
    IN      ULONG                           ProcessFlags,                   // PROCESS_CREATE_FLAGS_*
    IN      ULONG                           ThreadFlags,                    // THREAD_CREATE_FLAGS_*
    IN      PRTL_USER_PROCESS_PARAMETERS    ProcessParameters,
    IN OUT  PPS_CREATE_INFO                 CreateInfo,
    IN      PPS_ATTRIBUTE_LIST              AttributeList
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenProcess(
    PHANDLE             ProcessHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PCLIENT_ID          ClientId
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenProcess(
    PHANDLE             ProcessHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PCLIENT_ID          ClientId
);

NATIVE_API
NTSTATUS
NTAPI
NtTerminateProcess(
    IN HANDLE ProcessHandle,
    IN NTSTATUS ExitStatus
);

NATIVE_API
NTSTATUS
NTAPI
ZwTerminateProcess(
    IN HANDLE ProcessHandle,
    IN NTSTATUS ExitStatus
);

NATIVE_API
NTSTATUS
NTAPI
NtSuspendProcess(
    HANDLE ProcessHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwSuspendProcess(
    HANDLE ProcessHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtResumeProcess(
    HANDLE ProcessHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwResumeProcess(
    HANDLE ProcessHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtGetNextProcess(
    IN  HANDLE      ProcessHandle,
    IN  ACCESS_MASK DesiredAccess,
    IN  ULONG       HandleAttributes,
    IN  ULONG       Flags,
    OUT PHANDLE     NewProcessHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwGetNextProcess(
    IN  HANDLE      ProcessHandle,
    IN  ACCESS_MASK DesiredAccess,
    IN  ULONG       HandleAttributes,
    IN  ULONG       Flags,
    OUT PHANDLE     NewProcessHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtGetNextThread(
    IN  HANDLE      ProcessHandle,
    IN  HANDLE      ThreadHandle,
    IN  ACCESS_MASK DesiredAccess,
    IN  ULONG       HandleAttributes,
    IN  ULONG       Flags,
    OUT PHANDLE     NewThreadHandle
);

NATIVE_API
NTSTATUS
NTAPI
ZwGetNextThread(
    IN  HANDLE      ProcessHandle,
    IN  HANDLE      ThreadHandle,
    IN  ACCESS_MASK DesiredAccess,
    IN  ULONG       HandleAttributes,
    IN  ULONG       Flags,
    OUT PHANDLE     NewThreadHandle
);

/************************************************************************
  threads api
************************************************************************/

typedef struct
{
/* 0x000 */ ULONG   Flags;
/* 0x004 */ PSTR    FrameName;

} TEB_ACTIVE_FRAME_CONTEXT, *PTEB_ACTIVE_FRAME_CONTEXT;

typedef struct TEB_ACTIVE_FRAME
{
/* 0x000 */ ULONG                       Context;  // Flags;
/* 0x004 */ struct TEB_ACTIVE_FRAME    *Previous;
            ULONG_PTR                   Data;
// /* 0x008 */ PTEB_ACTIVE_FRAME_CONTEXT   Context;

#if CPP_DEFINED

    BOOL NeedPop;

    TEB_ACTIVE_FRAME(ULONG Context = 0)
    {
        NeedPop = FALSE;
        this->Context = Context;
        this->Data = 0;
    }

    ~TEB_ACTIVE_FRAME()
    {
        if (NeedPop)
            Pop();
    }

    VOID Push();
    VOID Pop();

#endif // cpp

} TEB_ACTIVE_FRAME, *PTEB_ACTIVE_FRAME;

NATIVE_API
VOID
NTAPI
RtlPushFrame(
    PTEB_ACTIVE_FRAME Frame
);

NATIVE_API
PTEB_ACTIVE_FRAME
NTAPI
RtlGetFrame(
    VOID
);

NATIVE_API
VOID
NTAPI
RtlPopFrame(
    PTEB_ACTIVE_FRAME Frame
);

#if CPP_DEFINED

inline VOID TEB_ACTIVE_FRAME::Push()
{
    NeedPop = TRUE;
    RtlPushFrame(this);
}

inline VOID TEB_ACTIVE_FRAME::Pop()
{
    NeedPop = FALSE;
    RtlPopFrame(this);
}

#endif // cpp

NATIVE_API
NTSTATUS
NTAPI
NtGetContextThread(
    IN  HANDLE ThreadHandle,
    OUT PCONTEXT Context
);

NATIVE_API
NTSTATUS
NTAPI
NtSetContextThread(
    IN HANDLE ThreadHandle,
    IN PCONTEXT Context
);

NATIVE_API
NTSTATUS
NTAPI
ZwGetContextThread(
    IN  HANDLE ThreadHandle,
    OUT PCONTEXT Context
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetContextThread(
    IN HANDLE ThreadHandle,
    IN PCONTEXT Context
);

NATIVE_API
VOID
NTAPI
RtlExitUserThread(
    ULONG ExitCode
);

NATIVE_API
NTSTATUS
NTAPI
NtYieldExecution(
    VOID
);

NATIVE_API
NTSTATUS
NTAPI
ZwYieldExecution(
    VOID
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateThread(
    OUT PHANDLE             ThreadHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  HANDLE              ProcessHandle,
    OUT PCLIENT_ID          ClientId,
    IN  PCONTEXT            ThreadContext,
    IN  PUSER_STACK         UserStack,
    IN  BOOLEAN             CreateSuspended
);

NATIVE_API
NTSTATUS
NTAPI
ZwCreateThread(
    OUT PHANDLE             ThreadHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes,
    IN  HANDLE              ProcessHandle,
    OUT PCLIENT_ID          ClientId,
    IN  PCONTEXT            ThreadContext,
    IN  PINITIAL_TEB        InitialTeb,
    IN  BOOLEAN             CreateSuspended
);

NATIVE_API
NTSTATUS
NTAPI
NtCreateThreadEx(
    OUT PHANDLE             ThreadHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes    OPTIONAL,
    IN  HANDLE              ProcessHandle,
    IN  PVOID               StartRoutine,
    IN  PVOID               Argument            OPTIONAL,
    IN  ULONG               CreateFlags, // THREAD_CREATE_FLAGS_*
    IN  ULONG_PTR           ZeroBits            OPTIONAL,
    IN  ULONG_PTR           StackSize           OPTIONAL,
    IN  ULONG_PTR           MaximumStackSize    OPTIONAL,
    IN  PPS_ATTRIBUTE_LIST  AttributeList       OPTIONAL
);

NTSTATUS
NTAPI
ZwCreateThreadEx(
    OUT PHANDLE             ThreadHandle,
    IN  ACCESS_MASK         DesiredAccess,
    IN  POBJECT_ATTRIBUTES  ObjectAttributes OPTIONAL,
    IN  HANDLE              ProcessHandle,
    IN  PVOID               StartRoutine,
    IN  PVOID               Argument OPTIONAL,
    IN  ULONG               CreateFlags, // THREAD_CREATE_FLAGS_*
    IN  ULONG_PTR           ZeroBits OPTIONAL,
    IN  ULONG_PTR           StackSize OPTIONAL,
    IN  ULONG_PTR           MaximumStackSize OPTIONAL,
    IN  PPS_ATTRIBUTE_LIST  AttributeList OPTIONAL
);

typedef
NTSTATUS
(NTAPI
*PUSER_THREAD_START_ROUTINE)(
    IN PVOID ThreadParameter
);

NATIVE_API
NTSTATUS
NTAPI
RtlCreateUserThread(
    IN  HANDLE                      Process,
    IN  PSECURITY_DESCRIPTOR        ThreadSecurityDescriptor    OPTIONAL,
    IN  BOOLEAN                     CreateSuspended,
    IN  ULONG                       ZeroBits                    OPTIONAL,
    IN  ULONG_PTR                   MaximumStackSize            OPTIONAL,
    IN  ULONG_PTR                   CommittedStackSize          OPTIONAL,
    IN  PUSER_THREAD_START_ROUTINE  StartAddress,
    IN  PVOID                       Parameter                   OPTIONAL,
    OUT PHANDLE                     Thread                      OPTIONAL,
    OUT PCLIENT_ID                  ClientId                    OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtSuspendThread(
    IN  HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtResumeThread(
    IN  HANDLE ThreadHandle,
    OUT PULONG PreviousSuspendCount OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
NtAlertThread(
    IN HANDLE ThreadHandle
);

NATIVE_API
NTSTATUS
NTAPI
NtAlertResumeThread(
    IN  HANDLE  ThreadHandle,
    OUT PULONG  SuspendCount
);

NATIVE_API
NTSTATUS
NTAPI
NtOpenThread(
    PHANDLE             ThreadHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PCLIENT_ID          ClientId
);

NATIVE_API
NTSTATUS
NTAPI
ZwOpenThread(
    PHANDLE             ThreadHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PCLIENT_ID          ClientId
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryInformationThread(
    HANDLE          ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID           ThreadInformation,
    ULONG           ThreadInformationLength,
    PULONG          ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwQueryInformationThread(
    HANDLE          ThreadHandle,
    THREADINFOCLASS ThreadInformationClass,
    PVOID           ThreadInformation,
    ULONG           ThreadInformationLength,
    PULONG          ReturnLength
);

NATIVE_API
NTSTATUS
NTAPI
NtSetInformationThread(
    HANDLE                      ThreadHandle,
    THREADINFOCLASS    ThreadInformationClass,
    PVOID                       ThreadInformation,
    ULONG                       ThreadInformationLength
);

NATIVE_API
NTSTATUS
NTAPI
ZwSetInformationThread(
    HANDLE                      ThreadHandle,
    THREADINFOCLASS    ThreadInformationClass,
    PVOID                       ThreadInformation,
    ULONG                       ThreadInformationLength
);

NATIVE_API
NTSTATUS
NTAPI
NtTerminateThread(
    IN HANDLE   ThreadHandle,
    IN ULONG    ExitCode
);

NATIVE_API
NTSTATUS
NTAPI
ZwTerminateThread(
    IN HANDLE   ThreadHandle,
    IN ULONG    ExitCode
);

NATIVE_API
NTSTATUS
NTAPI
NtContinue(
    IN PCONTEXT Context,
    IN BOOLEAN  TestAlert
);

NATIVE_API
NTSTATUS
NTAPI
ZwContinue(
    IN PCONTEXT Context,
    IN BOOLEAN  TestAlert
);

NATIVE_API
NTSTATUS
NTAPI
RtlRemoteCall(
    IN  HANDLE      Process,
    IN  HANDLE      Thread,
    IN  PVOID       CallSite,
    IN  ULONG       ArgumentCount,
    IN  PULONG_PTR  Arguments OPTIONAL,
    IN  BOOLEAN     PassContext,
    IN  BOOLEAN     AlreadySuspended
);

NATIVE_API
NTSTATUS
NTAPI
NtQueueApcThread(
    IN HANDLE           ThreadHandle,
    IN PKNORMAL_ROUTINE ApcRoutine,
    IN PVOID            ApcRoutineContext OPTIONAL,
    IN PVOID            Argument1 OPTIONAL,
    IN PVOID            Argument2 OPTIONAL
);

NATIVE_API
VOID
NTAPI
RtlCaptureContext(
    OUT PCONTEXT ContextRecord
);

#if ML_KERNEL_MODE

NTKRNLAPI
PVOID
NTAPI
PsGetThreadWin32Thread(
    PETHREAD Thread
);

NTKRNLAPI
NTSTATUS
NTAPI
PsGetContextThread(
    IN      PETHREAD        Thread,
    IN OUT  PCONTEXT        ThreadContext,
    IN      KPROCESSOR_MODE PreviousMode
);

NTKRNLAPI
NTSTATUS
NTAPI
PsSetContextThread(
    IN      PETHREAD        Thread,
    IN OUT  PCONTEXT        ThreadContext,
    IN      KPROCESSOR_MODE PreviousMode
);

NTKRNLAPI
VOID
NTAPI
KeInitializeApc(
    OUT PRKAPC              Apc,
    IN  PRKTHREAD           Thread,
    IN  KAPC_ENVIRONMENT    Environment,
    IN  PKKERNEL_ROUTINE    KernelRoutine,
    IN  PKRUNDOWN_ROUTINE   RundownRoutine OPTIONAL,
    IN  PKNORMAL_ROUTINE    NormalRoutine OPTIONAL,
    IN  KPROCESSOR_MODE     ApcMode OPTIONAL,
    IN  PVOID               NormalContext OPTIONAL
);

NTKRNLAPI
BOOLEAN
NTAPI
KeInsertQueueApc(
    IN OUT  PRKAPC Apc,
    IN      PVOID SystemArgument1 OPTIONAL,
    IN      PVOID SystemArgument2 OPTIONAL,
    IN      KPRIORITY Increment
);

NTKRNLAPI
NTSTATUS
NTAPI
PsLookupProcessThreadByCid(
    IN  PCLIENT_ID  Cid,
    OUT PEPROCESS*  Process OPTIONAL,
    OUT PETHREAD*   Thread
);

NTKRNLAPI
PVOID
PsGetProcessSectionBaseAddress(
    IN PEPROCESS Process
);

NTKRNLAPI
PCHAR
NTAPI
PsGetProcessImageFileName(
    IN PEPROCESS Process
);

NTKRNLAPI
PVOID
NTAPI
PsGetProcessDebugPort(
    IN PEPROCESS Process
);

NTKRNLAPI
PVOID
NTAPI
PsGetProcessSecurityPort(
    IN PEPROCESS Process
);

NTKRNLAPI
NTSTATUS
NTAPI
PsSetProcessSecurityPort(
    OUT PEPROCESS Process,
    IN  PVOID Port
);

NTKRNLAPI
NTSTATUS
NTAPI
PsGetProcessExitStatus(
    IN PEPROCESS Process
);

NTKRNLAPI
HANDLE
NTAPI
PsGetThreadProcessId(
    IN PETHREAD Thread
);

NTKRNLAPI
PEPROCESS
NTAPI
PsGetThreadProcess(
    IN PETHREAD Thread
);

NTKERNELAPI
PVOID
NTAPI
PsGetThreadTeb(
    PETHREAD Thread
);

NTKERNELAPI
NTSTATUS
NTAPI
PsSuspendProcess(
    PEPROCESS Process
);

NTKERNELAPI
NTSTATUS
NTAPI
PsResumeProcess(
    PEPROCESS Process
);

NTKRNLAPI
VOID
NTAPI
KiAttachProcess_NT5(
    IN OUT  PRKTHREAD           Thread,
    IN      PKPROCESS           Process,
    IN      PKLOCK_QUEUE_HANDLE LockHandle,
    OUT     PRKAPC_STATE        SavedApcState
);

NTKRNLAPI
VOID
NTAPI
KiAttachProcess_NT6(
    // IN OUT  PRKTHREAD           Thread, // eax
    IN      PKPROCESS       Process,
    IN      KIRQL           PreviousIrql,
    OUT     PRKAPC_STATE    SavedApcState
);

#endif // r0

#endif // _NTPROCESS_H_39560d0d_86be_4220_839a_0fcdb8e556cf_
