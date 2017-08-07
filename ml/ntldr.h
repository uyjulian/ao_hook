#ifndef _NTLDR_H_f6018b26_f2d3_44a1_b76f_f79cb5ff59dc_
#define _NTLDR_H_f6018b26_f2d3_44a1_b76f_f79cb5ff59dc_


//
// Private flags for loader data table entries
//

#define LDRP_STATIC_LINK                0x00000002
#define LDRP_IMAGE_DLL                  0x00000004
#define LDRP_LOAD_IN_PROGRESS           0x00001000
#define LDRP_UNLOAD_IN_PROGRESS         0x00002000
#define LDRP_ENTRY_PROCESSED            0x00004000
#define LDRP_ENTRY_INSERTED             0x00008000
#define LDRP_CURRENT_LOAD               0x00010000
#define LDRP_FAILED_BUILTIN_LOAD        0x00020000
#define LDRP_DONT_CALL_FOR_THREADS      0x00040000
#define LDRP_PROCESS_ATTACH_CALLED      0x00080000
#define LDRP_DEBUG_SYMBOLS_LOADED       0x00100000
#define LDRP_IMAGE_NOT_AT_BASE          0x00200000
#define LDRP_COR_IMAGE                  0x00400000
#define LDRP_COR_OWNS_UNMAP             0x00800000
#define LDRP_SYSTEM_MAPPED              0x01000000
#define LDRP_IMAGE_VERIFYING            0x02000000
#define LDRP_DRIVER_DEPENDENT_DLL       0x04000000
#define LDRP_ENTRY_NATIVE               0x08000000
#define LDRP_REDIRECTED                 0x10000000
#define LDRP_NON_PAGED_DEBUG_INFO       0x20000000
#define LDRP_MM_LOADED                  0x40000000
#define LDRP_COMPAT_DATABASE_PROCESSED  0x80000000

typedef struct _LDR_DATA_TABLE_ENTRY
{
/* +0x000 */ LIST_ENTRY     InLoadOrderLinks;
/* +0x008 */ LIST_ENTRY     InMemoryOrderLinks;
/* +0x010 */ LIST_ENTRY     InInitializationOrderLinks;
/* +0x018 */ PVOID          DllBase;
/* +0x01c */ PVOID          EntryPoint;
/* +0x020 */ ULONG          SizeOfImage;
/* +0x024 */ UNICODE_STRING FullDllName;
/* +0x02c */ UNICODE_STRING BaseDllName;
/* +0x034 */ ULONG          Flags;
/* +0x038 */ USHORT         LoadCount;
/* +0x03a */ USHORT         TlsIndex;

             union
             {
/* +0x03c */    LIST_ENTRY     HashLinks;
                struct
                {
/* +0x03c */        PVOID          SectionPointer;
/* +0x040 */        ULONG          CheckSum;
                };
             };

             union
             {
/* +0x044 */    ULONG          TimeDateStamp;
/* +0x044 */    PVOID          LoadedImports;
             };

/* +0x048 */ PVOID          EntryPointActivationContext;
/* +0x04c */ PVOID          PatchInformation;
/* +0x050 */ LIST_ENTRY     ForwarderLinks;
/* +0x058 */ LIST_ENTRY     ServiceTagLinks;
/* +0x060 */ LIST_ENTRY     StaticLinks;
/* +0x068 */ PVOID          ContextInformation;
/* +0x06c */ ULONG          OriginalBase;
/* +0x070 */ LARGE_INTEGER  LoadTime;

#if CPP_DEFINED

    ForceInline _LDR_DATA_TABLE_ENTRY* NextLoadOrder()
    {
        return FIELD_BASE(this->InLoadOrderLinks.Flink, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
    }

    ForceInline _LDR_DATA_TABLE_ENTRY* NextMemoryOrder()
    {
        return FIELD_BASE(this->InMemoryOrderLinks.Flink, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
    }

    ForceInline _LDR_DATA_TABLE_ENTRY* NextInitializationOrder()
    {
        return FIELD_BASE(this->InInitializationOrderLinks.Flink, _LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
    }

    ForceInline _LDR_DATA_TABLE_ENTRY* PrevLoadOrder()
    {
        return FIELD_BASE(this->InLoadOrderLinks.Blink, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
    }

    ForceInline _LDR_DATA_TABLE_ENTRY* PrevMemoryOrder()
    {
        return FIELD_BASE(this->InMemoryOrderLinks.Blink, _LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
    }

    ForceInline _LDR_DATA_TABLE_ENTRY* PrevInitializationOrder()
    {
        return FIELD_BASE(this->InInitializationOrderLinks.Blink, _LDR_DATA_TABLE_ENTRY, InInitializationOrderLinks);
    }

#endif // cpp

} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef const LDR_DATA_TABLE_ENTRY* PCLDR_DATA_TABLE_ENTRY;

typedef LDR_DATA_TABLE_ENTRY LDR_MODULE;
typedef PLDR_DATA_TABLE_ENTRY PLDR_MODULE;
typedef PCLDR_DATA_TABLE_ENTRY PCLDR_MODULE;


NATIVE_API
VOID
NTAPI
LdrShutdownProcess(
    VOID
);

NATIVE_API
VOID
NTAPI
LdrShutdownThread(
    VOID
);

#define LDR_ADDREF_DLL_PIN 0x00000001

NATIVE_API
NTSTATUS
NTAPI
LdrAddRefDll(
    ULONG Flags,
    PVOID DllHandle
);

NATIVE_API
NTSTATUS
NTAPI
LdrGetDllHandle(
    IN  PUSHORT         PathToFile OPTIONAL,
    IN  PULONG          DllCharacteristics OPTIONAL,
    IN  PUNICODE_STRING ModuleFileName,
    OUT PVOID*          DllHandle
);


#define LDR_GET_DLL_HANDLE_EX_UNCHANGED_REFCOUNT 0x00000001
#define LDR_GET_DLL_HANDLE_EX_PIN 0x00000002

NATIVE_API
NTSTATUS
NTAPI
LdrGetDllHandleEx(
    IN  ULONG           Flags,
    IN  PCWSTR          DllPath OPTIONAL,
    IN  PULONG          DllCharacteristics OPTIONAL,
    IN  PUNICODE_STRING DllName,
    OUT PVOID*          DllHandle OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
LdrGetDllHandleByMapping(
    IN  PVOID   Base,
    OUT PVOID*  DllHandle
);

NATIVE_API
NTSTATUS
NTAPI
LdrGetDllHandleByName(
    IN  PUNICODE_STRING BaseDllName OPTIONAL,
    IN  PUNICODE_STRING FullDllName OPTIONAL,
    OUT PVOID*          DllHandle
);

NATIVE_API
NTSTATUS
NTAPI
LdrLoadDll(
    IN  PWSTR               PathToFile OPTIONAL,
    IN  PULONG              DllCharacteristics OPTIONAL,
    IN  PCUNICODE_STRING    ModuleFileName,
    OUT PVOID*              DllHandle
);

NATIVE_API
NTSTATUS
NTAPI
LdrUnloadDll(
    PVOID DllHandle
);

NATIVE_API
NTSTATUS
NTAPI
LdrGetProcedureAddress(
    IN  PVOID           DllHandle,
    IN  PANSI_STRING    ProcedureName OPTIONAL,
    IN  USHORT          ProcedureNumber OPTIONAL,
    OUT PVOID*          ProcedureAddress
);

NATIVE_API
NTSTATUS
NTAPI
LdrDisableThreadCalloutsForDll(
    IN PVOID DllHandle
);

NATIVE_API
NTSTATUS
NTAPI
LdrFindEntryForAddress(
    IN  PVOID Address,
    OUT PLDR_DATA_TABLE_ENTRY *TableEntry
);

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
/* 0x000 */ HANDLE                              Section;
/* 0x004 */ PVOID                               MappedBase;
/* 0x008 */ PVOID                               ImageBase;
/* 0x00C */ ULONG                               ImageSize;
/* 0x010 */ ULONG                               Flags;
/* 0x014 */ USHORT                              LoadOrderIndex;
/* 0x016 */ USHORT                              InitOrderIndex;
/* 0x018 */ USHORT                              LoadCount;
/* 0x01A */ USHORT                              OffsetToFileName;
/* 0x01C */ UCHAR                               FullPathName[MAXIMUM_FILENAME_LENGTH];

} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
/* 0x000 */ ULONG                               NumberOfModules;
/* 0x004 */ RTL_PROCESS_MODULE_INFORMATION      Modules[1];

} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

NATIVE_API
NTSTATUS
NTAPI
LdrQueryProcessModuleInformation(
    OUT PRTL_PROCESS_MODULES    ModuleInformation,
    IN  ULONG                   ModuleInformationLength,
    OUT PULONG                  ReturnLength OPTIONAL
);


typedef
VOID
(NTAPI
*PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION)(
    IN PCLDR_DATA_TABLE_ENTRY   DataTableEntry,
    IN PVOID                    Context,
    IN OUT PBOOLEAN             StopEnumeration
);

NTSYSAPI
NTSTATUS
NTAPI
LdrEnumerateLoadedModules(
    IN ULONG Flags,
    IN PLDR_LOADED_MODULE_ENUMERATION_CALLBACK_FUNCTION CallbackFunction,
    IN PVOID Context
);


#define LDR_RESOURCE_ID_NAME_MASK   ((~(ULONG_PTR)0) << 16) /* lower 16bits clear */
#define LDR_RESOURCE_ID_NAME_MINVAL (( (ULONG_PTR)1) << 16) /* 17th bit set */

//
// These are how you currently pass the flag to FindResource.
//
// VIEW_TO_DATAFILE and DATAFILE_TO_VIEW are idempotent,
// so you can covert a datafile to a datafile with VIEW_TO_DATAFILE.
// Think of better names therefore..
//
#define LDR_VIEW_TO_DATAFILE(x) ((PVOID)(((ULONG_PTR)(x)) |  (ULONG_PTR)1))
#define LDR_IS_DATAFILE(x)              (((ULONG_PTR)(x)) &  (ULONG_PTR)1)
#define LDR_IS_VIEW(x)                  (!LDR_IS_DATAFILE(x))
#define LDR_DATAFILE_TO_VIEW(x) ((PVOID)(((ULONG_PTR)(x)) & ~(ULONG_PTR)1))

//
// These functions work on ULONG, ULONG_PTR, ULONG64, etc.
// They do not work on pointers.
//
#define LDR_VIEW_TO_DATAFILE_INTEGER(x) ((x) | 1)
#define LDR_IS_DATAFILE_INTEGER(x)      (((x) & 1) == 1)
#define LDR_IS_VIEW_INTEGER(x)          (((x) & 1) == 0)
#define LDR_DATAFILE_TO_VIEW_INTEGER(x) ((x) - ((x) & 1))


//
// These are flags to a function that doesn't yet exist:
//    LdrpSearchResourceSectionEx and/or LdrpSearchOutOfProcessResourceSection
//
#define LDRP_FIND_RESOURCE_DATA                 (0x00000000)
#define LDRP_FIND_RESOURCE_DIRECTORY            (0x00000002)

//
// Flags to LdrFindResourceEx/LdrpSearchResourceSection/LdrFindOutOfProcessResource.
//
#define LDR_FIND_RESOURCE_LANGUAGE_CAN_FALLBACK            (0x00000000)
#define LDR_FIND_RESOURCE_LANGUAGE_EXACT                   (0x00000004)
#define LDR_FIND_RESOURCE_LANGUAGE_REDIRECT_VERSION        (0x00000008)

NATIVE_API
NTSTATUS
NTAPI
LdrFindResourceDirectory_U(
    IN PVOID                        DllHandle,
    IN CONST ULONG_PTR*             ResourceIdPath,
    IN ULONG                        ResourceIdPathLength,
    OUT PIMAGE_RESOURCE_DIRECTORY*  ResourceDirectory
);

NATIVE_API
NTSTATUS
NTAPI
LdrFindResource_U(
    IN  PVOID                       DllHandle,
    IN  PULONG_PTR                  ResourceIdPath,
    IN  ULONG                       ResourceIdPathLength,
    OUT PIMAGE_RESOURCE_DATA_ENTRY* ResourceDataEntry
);

NATIVE_API
NTSTATUS
NTAPI
LdrFindResourceEx_U(
    IN  ULONG                       Flags,
    IN  PVOID                       DllHandle,
    IN  PULONG_PTR                  ResourceIdPath,
    IN  ULONG                       ResourceIdPathLength,
    OUT PIMAGE_RESOURCE_DATA_ENTRY* ResourceDataEntry
);

NATIVE_API
NTSTATUS
NTAPI
LdrResSearchResource(
    IN  PVOID       DllHandle,
    IN  PULONG_PTR  ResourceIdPath,
    IN  ULONG       ResourceIdPathLength,
    IN  ULONG       Flags,
    OUT PVOID*      Resource,
    OUT PULONG      Size,
        PVOID       Reserve1,
        PVOID       Reserve2
);

// type, id/name, langid
#define LDR_MAXIMUM_RESOURCE_PATH_DEPTH (3)

typedef struct _LDR_ENUM_RESOURCE_ENTRY {
    union {
        ULONG_PTR NameOrId;
        PIMAGE_RESOURCE_DIRECTORY_STRING Name;
        struct {
            USHORT Id;
            USHORT NameIsPresent;
        };
    } Path[ LDR_MAXIMUM_RESOURCE_PATH_DEPTH ];
    PVOID Data;
    ULONG Size;
    ULONG Reserved;
} LDR_ENUM_RESOURCE_ENTRY, *PLDR_ENUM_RESOURCE_ENTRY;

NTSYSAPI
NTSTATUS
NTAPI
LdrEnumResources(
    IN PVOID                        DllHandle,
    IN PULONG_PTR                   ResourceIdPath,
    IN ULONG                        ResourceIdPathLength,
    IN OUT PULONG                   NumberOfResources,
    OUT PLDR_ENUM_RESOURCE_ENTRY    Resources OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
LdrAccessResource(
    IN  PVOID                       DllHandle,
    IN  PIMAGE_RESOURCE_DATA_ENTRY  ResourceDataEntry,
    OUT PVOID*                      Address OPTIONAL,
    OUT PULONG                      Size OPTIONAL
);

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {
    ULONG               Flags;          // Reserved.
    PCUNICODE_STRING    FullDllName;    // The full path name of the DLL module.
    PCUNICODE_STRING    BaseDllName;    // The base file name of the DLL module.
    PVOID               DllBase;        // A pointer to the base address for the DLL in memory.
    ULONG               SizeOfImage;    // The size of the DLL image, in bytes.
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
    ULONG               Flags;          // Reserved.
    PCUNICODE_STRING    FullDllName;    // The full path name of the DLL module.
    PCUNICODE_STRING    BaseDllName;    // The base file name of the DLL module.
    PVOID               DllBase;        // A pointer to the base address for the DLL in memory.
    ULONG               SizeOfImage;    // The size of the DLL image, in bytes.
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
    LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
    LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef const LDR_DLL_NOTIFICATION_DATA* PCLDR_DLL_NOTIFICATION_DATA;

#define LDR_DLL_NOTIFICATION_REASON_LOADED      1
#define LDR_DLL_NOTIFICATION_REASON_UNLOADED    2

typedef
VOID
(NTAPI
*PLDR_DLL_NOTIFICATION_FUNCTION)(
    IN  ULONG                       NotificationReason,
    IN  PCLDR_DLL_NOTIFICATION_DATA NotificationData,
    IN  PVOID                       Context OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
LdrRegisterDllNotification(
    IN  ULONG                           Flags,
    IN  PLDR_DLL_NOTIFICATION_FUNCTION  NotificationFunction,
    IN  PVOID                           Context OPTIONAL,
    OUT PVOID*                          Cookie
);

NATIVE_API
NTSTATUS
NTAPI
LdrUnregisterDllNotification(
    IN PVOID Cookie
);

#define LDR_LOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS               (0x00000001)
#define LDR_LOCK_LOADER_LOCK_FLAG_TRY_ONLY                      (0x00000002)

#define LDR_LOCK_LOADER_LOCK_DISPOSITION_INVALID                (0)
#define LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_ACQUIRED          (1)
#define LDR_LOCK_LOADER_LOCK_DISPOSITION_LOCK_NOT_ACQUIRED      (2)

NATIVE_API
NTSTATUS
NTAPI
LdrLockLoaderLock(
    IN  ULONG   Flags,
    OUT PULONG  Disposition OPTIONAL,
    OUT PVOID*  Cookie
);

#define LDR_UNLOCK_LOADER_LOCK_FLAG_RAISE_ON_ERRORS             (0x00000001)

NATIVE_API
NTSTATUS
NTAPI
LdrUnlockLoaderLock(
    IN      ULONG Flags,
    IN OUT  PVOID Cookie
);

/************************************************************************
  ml C interface
************************************************************************/
PIMAGE_RESOURCE_DATA_ENTRY
Nt_FindResource(
    PVOID   Module,
    PCWSTR  Name,
    PCWSTR  Type
);

PVOID
Nt_LoadResource(
    PVOID                       Module,
    PIMAGE_RESOURCE_DATA_ENTRY  ResourceDataEntry,
    PULONG                      DEF_VAL(Size, NULL)
);

#endif // _NTLDR_H_f6018b26_f2d3_44a1_b76f_f79cb5ff59dc_