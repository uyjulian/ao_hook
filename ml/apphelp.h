#ifndef _APPHELP_H_982410c2_a553_457c_990e_c996cff3e025
#define _APPHELP_H_982410c2_a553_457c_990e_c996cff3e025


#pragma INCLUDE_LIB(undoc_apphelp.lib)

_ML_C_HEAD_

#define SDBGMEF_IGNORE_ENVIRONMENT  1

#define SDB_DATABASE_MAIN           0x80000000
#define SDB_DATABASE_SHIM           0x00010000
#define SDB_DATABASE_MSI            0x00020000
#define SDB_DATABASE_DRIVERS        0x00040000
#define SDB_DATABASE_DETAILS        0x00080000
#define SDB_DATABASE_SP_DETAILS     0x00100000
#define SDB_DATABASE_RESOURCE       0x00200000
#define SDB_DATABASE_TYPE_MASK      0xF02F0000

#define SDB_DATABASE_MAIN_SHIM              (SDB_DATABASE_SHIM          | SDB_DATABASE_MSI | SDB_DATABASE_MAIN)
#define SDB_DATABASE_MAIN_MSI               (SDB_DATABASE_MSI           | SDB_DATABASE_MAIN)
#define SDB_DATABASE_MAIN_DRIVERS           (SDB_DATABASE_DRIVERS       | SDB_DATABASE_MAIN)
#define SDB_DATABASE_MAIN_DETAILS           (SDB_DATABASE_DETAILS       | SDB_DATABASE_MAIN)
#define SDB_DATABASE_MAIN_SP_DETAILS        (SDB_DATABASE_SP_DETAILS    | SDB_DATABASE_MAIN)
#define SDB_DATABASE_MAIN_RESOURCE          (SDB_DATABASE_RESOURCE      | SDB_DATABASE_MAIN)

#define SDB_TAG_TYPE_NULL       0x1000
#define SDB_TAG_TYPE_BYTE       0x2000
#define SDB_TAG_TYPE_WORD       0x3000
#define SDB_TAG_TYPE_DWORD      0x4000
#define SDB_TAG_TYPE_QWORD      0x5000
#define SDB_TAG_TYPE_STRINGREF  0x6000
#define SDB_TAG_TYPE_LIST       0x7000
#define SDB_TAG_TYPE_STRING     0x8000
#define SDB_TAG_TYPE_BINARY     0x9000

// It seems that this type is "masked"
#define SDB_TAG_TYPE_MASK       0xF000

// http://msdn2.microsoft.com/en-us/library/bb432487

#define SDB_TAG_DATABASE                    (0x0001 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_LIBRARY                     (0x0002 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_INEXCLUDE                   (0x0003 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_SHIM                        (0x0004 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_PATCH                       (0x0005 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_APP                         (0x0006 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_EXE                         (0x0007 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_MATCHING_FILE               (0x0008 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_SHIM_REF                    (0x0009 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_PATCH_REF                   (0x000A | SDB_TAG_TYPE_LIST)
#define SDB_TAG_LAYER                       (0x000B | SDB_TAG_TYPE_LIST)
#define SDB_TAG_FILE                        (0x000C | SDB_TAG_TYPE_LIST)
#define SDB_TAG_APPHELP                     (0x000D | SDB_TAG_TYPE_LIST)
#define SDB_TAG_LINK                        (0x000E | SDB_TAG_TYPE_LIST)
#define SDB_TAG_DATA                        (0x000F | SDB_TAG_TYPE_LIST)
#define SDB_TAG_MSI_TRANSFORM               (0x0010 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_MSI_TRANSFORM_REF           (0x0011 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_MSI_PACKAGE                 (0x0012 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_FLAG                        (0x0013 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_MSI_CUSTOM_ACTION           (0x0014 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_FLAG_REF                    (0x0015 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_ACTION                      (0x0016 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_LOOKUP                      (0x0017 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_STRINGTABLE                 (0x0801 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_INDEXES                     (0x0802 | SDB_TAG_TYPE_LIST)
#define SDB_TAG_INDEX                       (0x0803 | SDB_TAG_TYPE_LIST)

#define SDB_TAG_NAME                        (0x0001 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_DESCRIPTION                 (0x0002 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_MODULE                      (0x0003 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_API                         (0x0004 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_VENDOR                      (0x0005 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_APP_NAME                    (0x0006 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_COMMAND_LINE                (0x0008 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_COMPANY_NAME                (0x0009 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_DLLFILE                     (0x000A | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_WILDCARD_NAME               (0x000B | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_PRODUCT_NAME                (0x0010 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_PRODUCT_VERSION             (0x0011 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_FILE_DESCRIPTION            (0x0012 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_FILE_VERSION                (0x0013 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_ORIGINAL_FILENAME           (0x0014 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_INTERNAL_NAME               (0x0015 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_LEGAL_COPYRIGHT             (0x0016 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_16BIT_DESCRIPTION           (0x0017 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_APPHELP_DETAILS             (0x0018 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_LINK_URL                    (0x0019 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_LINK_TEXT                   (0x001A | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_APPHELP_TITLE               (0x001B | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_APPHELP_CONTACT             (0x001C | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_SXS_MANIFEST                (0x001D | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_DATA_STRING                 (0x001E | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_MSI_TRANSFORM_FILE          (0x001F | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_16BIT_MODULE_NAME           (0x0020 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_LAYER_DISPLAYNAME           (0x0021 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_COMPILER_VERSION            (0x0022 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_ACTION_TYPE                 (0x0023 | SDB_TAG_TYPE_STRINGREF)
#define SDB_TAG_EXPORT_NAME                 (0x0024 | SDB_TAG_TYPE_STRINGREF)

#define SDB_TAG_SIZE                        (0x0001 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_OFFSET                      (0x0002 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_CHECKSUM                    (0x0003 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_SHIM_TAGID                  (0x0004 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PATCH_TAGID                 (0x0005 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_MODULE_TYPE                 (0x0006 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VERDATEHI                   (0x0007 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VERDATELO                   (0x0008 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VERFILEOS                   (0x0009 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VERFILETYPE                 (0x000A | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PE_CHECKSUM                 (0x000B | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PREVOSMAJORVER              (0x000C | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PREVOSMINORVER              (0x000D | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PREVOSPLATFORMID            (0x000E | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PREVOSBUILDNO               (0x000F | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PROBLEMSEVERITY             (0x0010 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_LANGID                      (0x0011 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VER_LANGUAGE                (0x0012 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_ENGINE                      (0x0014 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_HTMLHELPID                  (0x0015 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_INDEX_FLAGS                 (0x0016 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_FLAGS                       (0x0017 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_DATA_VALUETYPE              (0x0018 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_DATA_DWORD                  (0x0019 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_LAYER_TAGID                 (0x001A | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_MSI_TRANSFORM_TAGID         (0x001B | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_LINKER_VERSION              (0x001C | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_LINK_DATE                   (0x001D | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_UPTO_LINK_DATE              (0x001E | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_OS_SERVICE_PACK             (0x001F | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_FLAG_TAGID                  (0x0020 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_RUNTIME_PLATFORM            (0x0021 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_OS_SKU                      (0x0022 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_OS_PLATFORM                 (0x0023 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_APP_NAME_RC_ID              (0x0024 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VENDOR_NAME_RC_ID           (0x0025 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_SUMMARY_MSG_RC_ID           (0x0026 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_VISTA_SKU                   (0x0027 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_DESCRIPTION_RC_ID           (0x0028 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_PARAMETER1_RC_ID            (0x0029 | SDB_TAG_TYPE_DWORD)
#define SDB_TAG_TAGID                       (0x0801 | SDB_TAG_TYPE_DWORD)

#define SDB_TAG_STRINGTABLE_ITEM            (0x0801 | SDB_TAG_TYPE_STRING)

#define SDB_TAG_INCLUDE                     (0x0001 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_GENERAL                     (0x0002 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_MATCH_LOGIC_NOT             (0x0003 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_APPLY_ALL_SHIMS             (0x0004 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_USE_SERVICE_PACK_FILES      (0x0005 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_MITIGATION_OS               (0x0006 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_BLOCK_UPGRADE               (0x0007 | SDB_TAG_TYPE_NULL)
#define SDB_TAG_INCLUDEEXCLUDEDLL           (0x0008 | SDB_TAG_TYPE_NULL)

#define SDB_TAG_TIME                        (0x0001 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_BIN_FILE_VERSION            (0x0002 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_BIN_PRODUCT_VERSION         (0x0003 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_MODTIME                     (0x0004 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_MASK_KERNEL            (0x0005 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_UPTO_BIN_PRODUCT_VERSION    (0x0006 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_DATA_QWORD                  (0x0007 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_MASK_USER              (0x0008 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAGS_NTVDM1                (0x0009 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAGS_NTVDM2                (0x000A | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAGS_NTVDM3                (0x000B | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_MASK_SHELL             (0x000C | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_UPTO_BIN_FILE_VERSION       (0x000D | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_MASK_FUSION            (0x000E | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_PROCESSPARAM           (0x000F | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_LUA                    (0x0010 | SDB_TAG_TYPE_QWORD)
#define SDB_TAG_FLAG_INSTALL                (0x0011 | SDB_TAG_TYPE_QWORD)

#define SDB_TAG_PATCH_BITS                  (0x0002 | SDB_TAG_TYPE_BINARY)
#define SDB_TAG_FILE_BITS                   (0x0003 | SDB_TAG_TYPE_BINARY)
#define SDB_TAG_EXE_ID                      (0x0004 | SDB_TAG_TYPE_BINARY)
#define SDB_TAG_DATA_BITS                   (0x0005 | SDB_TAG_TYPE_BINARY)
#define SDB_TAG_MSI_PACKAGE_ID              (0x0006 | SDB_TAG_TYPE_BINARY)
#define SDB_TAG_DATABASE_ID                 (0x0007 | SDB_TAG_TYPE_BINARY)
#define SDB_TAG_INDEX_BITS                  (0x0801 | SDB_TAG_TYPE_BINARY)

#define SDB_TAG_MATCH_MODE                  (0x0001 | SDB_TAG_TYPE_WORD)
#define SDB_TAG_TAG                         (0x0801 | SDB_TAG_TYPE_WORD)
#define SDB_TAG_INDEX_TAG                   (0x0802 | SDB_TAG_TYPE_WORD)
#define SDB_TAG_INDEX_KEY                   (0x0803 | SDB_TAG_TYPE_WORD)

typedef enum
{
    DOS_PATH,
    NT_PATH,

} PATH_TYPE;


#define SDB_TAGID_NULL (0)
#define SDB_TAGID_ROOT (0)

typedef ULONG TAGID;

typedef ULONG TAGREF, INDEXID, TAG;
typedef PVOID HSDB;

#pragma pack(1)

typedef struct tagAPPHELP_DATA
{
    ULONG  Flags;
    ULONG  Severity;
    ULONG  HTMLHelpID;
    LPWSTR AppName;
    TAGREF Exe;
    LPWSTR URL;
    LPWSTR Link;
    LPWSTR AppTitle;
    LPWSTR Contact;
    LPWSTR Details;
    ULONG  Data;
    BOOL   SPEntry;

} APPHELP_DATA, *PAPPHELP_DATA;

typedef struct tagATTRINFO
{
  TAG   AttrID;
  ULONG Flags;

  union
  {
    ULONGLONG   Attr64;
    ULONG       Attr32;
    PWCHAR      AttrString;
  };
} ATTRINFO, *PATTRINFO;

typedef struct _FIND_INFO
{
    TAGID     tiIndex;
    TAGID     tiCurrent;
    TAGID     tiEndIndex;
    TAG       tName;
    ULONG     dwIndexRec;
    ULONG     dwFlags;
    ULONGLONG ullKey;
    union
    {
        LPCWSTR szName;
        ULONG   dwName;
        GUID    *pguidName;
    };
} FIND_INFO, *PFIND_INFO;

#define SDB_MAX_EXES    16
#define SDB_MAX_LAYERS  8
#define SDB_MAX_SDBS    16

// Flags

#define SHIMREG_DISABLE_SHIM    (0x00000001u)
#define SHIMREG_DISABLE_APPHELP (0x00000002u)
#define SHIMREG_APPHELP_NOUI    (0x00000004u)
#define SHIMREG_APPHELP_CANCEL  (0x10000000u)
#define SHIMREG_DISABLE_SXS     (0x00000010u)
#define SHIMREG_DISABLE_LAYER   (0x00000020u)
#define SHIMREG_DISABLE_DRIVER  (0x00000040u)

typedef struct tagSDBQUERYRESULT
{
    TAGREF atrExes[SDB_MAX_EXES];
    ULONG  adwExeFlags[SDB_MAX_EXES];
    TAGREF atrLayers[SDB_MAX_LAYERS];
    ULONG  dwLayerFlags;
    TAGREF trApphelp;
    ULONG  dwExeCount;
    ULONG  dwLayerCount;
    GUID   guidID;
    ULONG  dwFlags;
    ULONG  dwCustomSDBMap;
    GUID   rgGuidDB[SDB_MAX_SDBS];
} SDBQUERYRESULT, *PSDBQUERYRESULT;

typedef struct      // 0x1E8, win7 ultimate SP1 x86
{
    HANDLE  DataBaseFileHandle;
    PVOID   MappedBase;
    ULONG   Unknown;
    ULONG   FileSize;
    BYTE    DataBaseId[0x14];

    HANDLE  SectionHandle;  // 0x1B8

} DB, *PDB;

typedef struct
{
    ULONG   Flags;
    GUID    Guid;
} SDB_DATABASE_INFORMATION, *PSDB_DATABASE_INFORMATION;

#pragma pack()

EXTC_IMPORT BOOL WINAPI BaseFlushAppcompatCache();

EXTC_IMPORT
VOID
WINAPI
SdbCloseDatabase(
    IN  PDB Db
);

EXTC_IMPORT
VOID
WINAPI
SdbCloseDatabaseWrite(
    IN  PDB Db
);

EXTC_IMPORT
BOOL
WINAPI
SdbCommitIndexes(
    IN  PDB Db
);

EXTC_IMPORT
PDB
WINAPI
SdbCreateDatabase(
    IN  PCWSTR      DataBase,
    IN  PATH_TYPE   Type
);

EXTC_IMPORT
BOOL
WINAPI
SdbDeclareIndex(
    IN  PDB pdb,
    IN  TAG tWhich,
    IN  TAG tKey,
    IN  ULONG dwEntries,
    IN  BOOL bUniqueKey,
    OUT INDEXID *piiIndex
);

EXTC_IMPORT
TAGID
WINAPI
SdbBeginWriteListTag(
    IN PDB      Db,
    IN TAGID    ListTag
);

EXTC_IMPORT
BOOL
WINAPI
SdbEndWriteListTag(
    IN OUT  PDB     Db,
    IN      TAGID   ListTag
);

EXTC_IMPORT
TAGID
WINAPI
SdbFindFirstDWORDIndexedTag(
    IN  PDB pdb,
    IN  TAG tWhich,
    IN  TAG tKey,
    IN  ULONG dwName,
    OUT FIND_INFO *pFindInfo
);

EXTC_IMPORT
TAGID
WINAPI
SdbFindFirstTag(
    IN  PDB pdb,
    IN  TAGID tiParent,
    IN  TAG tTag
);

EXTC_IMPORT
TAGID
WINAPI
SdbFindFirstNamedTag(
    IN  PDB     Db,
    IN  TAGID   TagId,
    IN  TAGID   Parent,
    IN  TAG     Tag
);

EXTC_IMPORT
TAGREF
WINAPI
SdbFindFirstTagRef(
    IN  PDB     Db,
    IN  TAGREF  TagRef,
    IN  TAG     Tag
);

EXTC_IMPORT
TAGID
WINAPI
SdbFindNextTag(
    IN  PDB pdb,
    IN  TAGID tiParent,
    IN  TAGID tiPrev
);

EXTC_IMPORT
BOOL
WINAPI
SdbFormatAttribute(
    IN  PATTRINFO pAttrInfo,
    OUT LPWSTR pchBuffer,
    IN  ULONG dwBufferSize
);

EXTC_IMPORT
BOOL
WINAPI
SdbFreeFileAttributes(
    IN  PATTRINFO pFileAttributes
);

EXTC_IMPORT
VOID
WINAPI
SdbGetAppPatchDir(
    IN  HSDB hSDB OPTIONAL,
    OUT LPWSTR szAppPatchPath,
    IN  ULONG cchSize
);

EXTC_IMPORT
PVOID
WINAPI
SdbGetBinaryTagData(
    IN  PDB     Db,
    IN  TAGID   Which
);

EXTC_IMPORT
BOOL
WINAPI
SdbGetDatabaseID(
    IN  PDB     Db,
    OUT GUID   *Guid
);

EXTC_IMPORT
BOOL
WINAPI
SdbGetDatabaseGUID(
    IN  PDB     Db,
    OUT GUID   *Guid
);

EXTC_IMPORT
BOOL
WINAPI
SdbGetDatabaseInformation(
    IN  PDB                         Db,
    OUT PSDB_DATABASE_INFORMATION   DatabaseInformation
);

EXTC_IMPORT
BOOL
WINAPI
SdbGetFileAttributes(
    IN  LPCWSTR lpwszFileName,
    OUT PATTRINFO *ppAttrInfo,
    OUT LPDWORD lpdwAttrCount
);

EXTC_IMPORT
TAGID
WINAPI
SdbGetFirstChild(
    IN  PDB     Db,
    IN  TAGID   ParentTagId
);

EXTC_IMPORT
TAGID
WINAPI
SdbGetIndex(
    IN  PDB pdb,
    IN  TAG tWhich,
    IN  TAG tKey,
    OUT LPDWORD lpdwFlags OPTIONAL
);

EXTC_IMPORT
BOOL
WINAPI
SdbGetMatchingExe(
    IN  HSDB    hSDB OPTIONAL,
    IN  LPCWSTR szPath,
    IN  LPCWSTR szModuleName OPTIONAL,
    IN  LPCWSTR pszEnvironment OPTIONAL,
    IN  ULONG dwFlags,
    OUT PSDBQUERYRESULT pQueryResult
);

EXTC_IMPORT
TAGID
WINAPI
SdbGetNextChild(
    IN  PDB pdb,
    IN  TAGID tiParent,
    IN  TAGID tiPrev
);

EXTC_IMPORT
PWSTR
WINAPI
SdbGetStringTagPtr(
    IN  PDB pdb,
    IN  TAGID WhichTagId
);

EXTC_IMPORT
TAG
WINAPI
SdbGetTagFromTagID(
    IN  PDB pdb,
    IN  TAGID tiWhich
);

EXTC_IMPORT
HSDB
WINAPI
SdbInitDatabase(
    IN  ULONG   Flags,
    IN  PCWSTR  DatabasePath
);

EXTC_IMPORT
BOOL
WINAPI
SdbIsStandardDatabase(
    IN  GUID GuidDB
);

EXTC_IMPORT
ULONGLONG
WINAPI
SdbMakeIndexKeyFromString(
    IN  LPCWSTR pwszKey
);

EXTC_IMPORT
PDB
WINAPI
SdbOpenApphelpDetailsDatabase(
    IN OUT  LPCWSTR pwsDetailsDatabasePath
);

EXTC_IMPORT
HMODULE
WINAPI
SdbOpenApphelpResourceFile(
    IN  LPCWSTR pwszACResourceFile OPTIONAL
);

EXTC_IMPORT
PDB
WINAPI
SdbOpenDatabase(
    IN  LPCWSTR pwszPath,
    IN  PATH_TYPE eType
);

EXTC_IMPORT
ULONG
WINAPI
SdbQueryDataExTagID(
    IN      PDB pdb,
    IN      TAGID tiExe,
    IN      LPCWSTR lpszDataName OPTIONAL,
    OUT     LPDWORD lpdwDataType OPTIONAL,
    OUT     LPVOID lpBuffer OPTIONAL,
    IN OUT  LPDWORD lpcbBufferSize OPTIONAL,
    OUT     TAGID *ptiData OPTIONAL
);

EXTC_IMPORT
BOOL
WINAPI
SdbReadApphelpDetailsData(
    IN  PDB pdb,
    OUT PAPPHELP_DATA pData
);

EXTC_IMPORT
BOOL
WINAPI
SdbReadBinaryTag(
    IN  PDB     Db,
    IN  TAGID   WhichTagId,
    OUT PBYTE   Buffer,
    IN  ULONG   BufferSize
);

EXTC_IMPORT
ULONG
WINAPI
SdbReadDWORDTag(
    IN  PDB pdb,
    IN  TAGID tiWhich,
    IN  ULONG dwDefault
);

EXTC_IMPORT
ULONGLONG
WINAPI
SdbReadQWORDTag(
    IN  PDB pdb,
    IN  TAGID tiWhich,
    IN  ULONGLONG qwDefault
);

EXTC_IMPORT
BOOL
WINAPI
SdbReadStringTag(
    IN  PDB pdb,
    IN  TAGID tiWhich,
    OUT LPWSTR pwszBuffer,
    IN  ULONG cchBufferSize
);

EXTC_IMPORT
BOOL
WINAPI
SdbRegisterDatabaseEx(
    IN  LPCWSTR pszDatabasePath,
    IN  ULONG dwDatabaseType,
    IN  PULONGLONG pTimeStamp OPTIONAL
);

EXTC_IMPORT
VOID
WINAPI
SdbReleaseDatabase(
    IN  HSDB hSDB
);

EXTC_IMPORT
VOID
WINAPI
SdbReleaseMatchingExe(
    IN  HSDB hSDB,
    IN  TAGREF trExe
);

EXTC_IMPORT
BOOL
WINAPI
SdbStartIndexing(
    IN  PDB pdb,
    IN  INDEXID iiWhich
);

EXTC_IMPORT
BOOL
WINAPI
SdbStopIndexing(
    IN  PDB pdb,
    IN  INDEXID WhichIndexId
);

EXTC_IMPORT
BOOL
WINAPI
SdbTagIDToTagRef(
    IN  HSDB    hSDB,
    IN  PDB     Db,
    IN  TAGID   TagId,
    OUT TAGREF *TagRef
);

EXTC_IMPORT
BOOL
WINAPI
SdbTagRefToTagID(
    IN  HSDB    hSDB,
    IN  TAGREF  WhichTagRef,
    OUT PDB    *Db,
    OUT TAGID  *WhichTagId
);

EXTC_IMPORT
LPCWSTR
WINAPI
SdbTagToString(
    IN  TAG tag
);

EXTC_IMPORT
BOOL
WINAPI
SdbUnregisterDatabase(
    IN  GUID *pguidDB
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteBinaryTag(
    IN  PDB     Db,
    IN  TAG     Tag,
    IN  PVOID   Buffer,
    IN  ULONG   Size
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteBinaryTagFromFile(
    IN  PDB     Db,
    IN  TAG     Tag,
    IN  PCWSTR  FileName
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteDWORDTag(
    IN  PDB     Db,
    IN  TAG     Tag,
    IN  ULONG   Data
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteNULLTag(
    IN  PDB Db,
    IN  TAG Tag
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteQWORDTag(
    IN  PDB     Db,
    IN  TAG     Tag,
    IN  ULONG64 Data
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteStringTag(
    IN  PDB     Db,
    IN  TAG     Tag,
    IN  PCWSTR  Data
);

EXTC_IMPORT
BOOL
WINAPI
SdbWriteWORDTag(
    IN  PDB     Db,
    IN  TAG     Tag,
    IN  USHORT  Data
);

EXTC_IMPORT
BOOL
WINAPI
ShimFlushCache(
    IN OUT  HWND hwnd,
    IN OUT  HINSTANCE hInstance,
    IN OUT  LPCSTR lpszCmdLine,
    IN      int nCmdShow
);

_ML_C_TAIL_

#endif // _APPHELP_H_982410c2_a553_457c_990e_c996cff3e025