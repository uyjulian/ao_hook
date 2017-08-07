#ifndef _NTNLS_H_66774c20_dc28_4a7e_9714_c1f39fdb5d5f_
#define _NTNLS_H_66774c20_dc28_4a7e_9714_c1f39fdb5d5f_

#include <ntnls.h>

NATIVE_API
VOID
NTAPI
RtlInitNlsTables(
    IN  PUSHORT         AnsiNlsBase,
    IN  PUSHORT         OemNlsBase,
    IN  PUSHORT         LanguageNlsBase,
    OUT PNLSTABLEINFO   TableInfo
);

NATIVE_API
VOID
NTAPI
RtlResetRtlTranslations(
    IN PNLSTABLEINFO TableInfo
);

NATIVE_API
NTSTATUS
NTAPI
NtInitializeNlsFiles(
    OUT PVOID*          BaseAddress,
    OUT PLCID           DefaultLocaleId,
    OUT PLARGE_INTEGER  DefaultCasingTableSize
);

NTSTATUS
NTAPI
NtGetNlsSectionPtr(
    IN  ULONG   SectionType,
    IN  ULONG   SectionData,
    IN  PVOID   ContextData,
    OUT PVOID*  SectionPointer,
    OUT PULONG  SectionSize
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryDefaultLocale(
    IN  BOOLEAN UserProfile,
    OUT PLCID   DefaultLocaleId
);

NATIVE_API
NTSTATUS
NTAPI
NtSetDefaultLocale(
    IN  BOOLEAN UserProfile,
    IN  LCID    DefaultLocaleId
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryDefaultUILanguage(
    OUT LANGID *DefaultUILanguageId
);

NATIVE_API
NTSTATUS
NTAPI
NtSetDefaultUILanguage(
    IN  LANGID DefaultUILanguageId
);

NATIVE_API
NTSTATUS
NTAPI
NtQueryInstallUILanguage(
    OUT LANGID *InstallUILanguageId
);



#if defined(_INC_SDKDDKVER) && _WIN32_WINNT_WIN7 >= 0x600

#include <ntnls.h>

NATIVE_API
VOID
NTAPI
RtlInitCodePageTable(
    IN  PUSHORT         TableBase,
    OUT PCPTABLEINFO    CodePageTable
);

NATIVE_API
NTSTATUS
NTAPI
RtlCustomCPToUnicodeN(
    IN  PCPTABLEINFO    CPTableInfo,
    OUT PWSTR           UnicodeString,
    IN  ULONG           MaxBytesInUnicodeString,
    OUT PULONG          BytesInUnicodeString OPTIONAL,
    IN  PCSTR           CustomCPString,
    IN  ULONG           BytesInCustomCPString
);

NATIVE_API
NTSTATUS
NTAPI
RtlUnicodeToCustomCPN(
    IN  PCPTABLEINFO    CPTableInfo,
    OUT PCHAR           CustomCPString,
    IN  ULONG           BytesInCustomCPString,
    OUT PULONG          BytesInMultiByteString OPTIONAL,
    IN  PCWSTR          UnicodeString,
    IN  ULONG           BytesInUnicodeString
);

#endif // SDK_VER

NATIVE_API
NTSTATUS
NTAPI
RtlMultiByteToUnicodeN(
    PWSTR   UnicodeString,
    ULONG   MaxBytesInUnicodeString,
    PULONG  BytesInUnicodeString OPTIONAL,
    PCSTR   MultiByteString,
    ULONG   BytesInMultiByteString
);

NATIVE_API
NTSTATUS
NTAPI
RtlUnicodeToMultiByteSize(
    PULONG  BytesInMultiByteString,
    PCWSTR  UnicodeString,
    ULONG   BytesInUnicodeString
);

NATIVE_API
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    OUT PSTR    MultiByteString,
    IN  ULONG   MaxBytesInMultiByteString,
    OUT PULONG  BytesInMultiByteString OPTIONAL,
    IN  PCWSTR  UnicodeString,
    IN  ULONG   BytesInUnicodeString
);

NATIVE_API
NTSTATUS
NTAPI
RtlMultiByteToUnicodeSize(
    OUT PULONG  BytesInUnicodeString,
    IN  PCSTR   MultiByteString,
    IN  ULONG   BytesInMultiByteString
);

NATIVE_API
NTSTATUS
NTAPI
RtlUnicodeToUTF8N(
    OUT PSTR    UTF8StringDestination,
    IN  ULONG   UTF8StringMaxByteCount,
    OUT PULONG  UTF8StringActualByteCount,
    IN  PCWSTR  UnicodeStringSource,
    IN  ULONG   UnicodeStringByteCount
);

NATIVE_API
NTSTATUS
NTAPI
RtlUTF8ToUnicodeN(
    OUT PWSTR   UnicodeStringDestination,
    IN  ULONG   UnicodeStringMaxByteCount,
    OUT PULONG  UnicodeStringActualByteCount,
    IN  PCSTR   UTF8StringSource,
    IN  ULONG   UTF8StringByteCount
);

NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToOemN(
    OUT PSTR    OemString,
    IN  ULONG   MaxBytesInOemString,
    OUT PULONG  BytesInOemString OPTIONAL,
    IN  PCWSTR  UnicodeString,
    IN  ULONG   BytesInUnicodeString
);

#endif // _NTNLS_H_66774c20_dc28_4a7e_9714_c1f39fdb5d5f_