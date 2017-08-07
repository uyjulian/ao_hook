#ifndef _NTSTRING_H_9bfab99e_9722_48ad_9a23_53edcb64be76_
#define _NTSTRING_H_9bfab99e_9722_48ad_9a23_53edcb64be76_

#include "ml/stringhelper.h"

#ifndef _NTIFS1_

NATIVE_API
NTSTATUS
NTAPI
RtlValidateUnicodeString(
    IN  ULONG Flags RESERVED,
    IN  PCUNICODE_STRING String
);

NATIVE_API
BOOLEAN
NTAPI
RtlCreateUnicodeString(
    OUT PUNICODE_STRING DestinationString,
    IN  PCWSTR          SourceString
);

NATIVE_API
VOID
NTAPI
RtlFreeAnsiString(
    PANSI_STRING AnsiString
);

NATIVE_API
VOID
NTAPI
RtlFreeUnicodeString(
    PUNICODE_STRING UnicodeString
);

NATIVE_API
VOID
NTAPI
RtlCopyUnicodeString(
    PUNICODE_STRING DestinationString,
    PCUNICODE_STRING SourceString
);

#define RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE             (0x00000001)
#define RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING       (0x00000002)
#define RTL_DUPSTR_ADD_NULL                                     RTL_DUPLICATE_UNICODE_STRING_NULL_TERMINATE
#define RTL_DUPSTR_ALLOC_NULL                                   RTL_DUPLICATE_UNICODE_STRING_ALLOCATE_NULL_STRING

NATIVE_API
NTSTATUS
NTAPI
RtlDuplicateUnicodeString(
    ULONG               Flags,
    PCUNICODE_STRING    SourceUnicodeString,
    PUNICODE_STRING     DestinationUnicodeString
);

NATIVE_API
BOOLEAN
NTAPI
RtlEqualUnicodeString(
    IN  PCUNICODE_STRING    String1,
    IN  PCUNICODE_STRING    String2,
    IN  BOOLEAN             CaseInSensitive
);

NATIVE_API
LONG
NTAPI
RtlCompareUnicodeString(
    PCUNICODE_STRING    String1,
    PCUNICODE_STRING    String2,
    BOOLEAN             CaseInSensitive
);

NATIVE_API
NTSTATUS
NTAPI
RtlUpcaseUnicodeString(
    IN OUT  PUNICODE_STRING     DestinationString,
    IN      PCUNICODE_STRING    SourceString,
    IN      BOOLEAN             AllocateDestinationString
);

NATIVE_API
NTSTATUS
NTAPI
RtlDowncaseUnicodeString(
    IN OUT  PUNICODE_STRING     DestinationString,
    IN      PCUNICODE_STRING    SourceString,
    IN      BOOLEAN             AllocateDestinationString
);

NATIVE_API
NTSTATUS
NTAPI
RtlAppendUnicodeToString(
    IN OUT  PUNICODE_STRING Destination,
    IN      PCWSTR          Source
);

NATIVE_API
NTSTATUS
NTAPI
RtlAppendUnicodeStringToString(
    IN OUT  PUNICODE_STRING     Destination,
    IN      PCUNICODE_STRING    Source
);

NATIVE_API
PWSTR
NTAPI
RtlFindUnicodeSubstring(
    IN PUNICODE_STRING Source,
    IN PUNICODE_STRING Substring
);

NATIVE_API
NTSTATUS
NTAPI
RtlMultiAppendUnicodeStringBuffer(
    IN OUT  PUNICODE_STRING Destination,
    IN      ULONG_PTR       NumberOfStringBuffer,
    IN      PWSTR*          StringBuffers
);

NATIVE_API
NTSTATUS
NTAPI
RtlStringFromGUID(
    IN  REFGUID Guid,
    OUT PUNICODE_STRING GuidString
);

NATIVE_API
NTSTATUS
NTAPI
RtlIntegerToUnicodeString(
    IN      ULONG           Value,
    IN      ULONG           Base OPTIONAL,
    IN OUT  PUNICODE_STRING String
);


#if !ML_KERNEL_MODE

NATIVE_API
NTSTATUS
NTAPI
RtlGUIDFromString(
    IN  PUNICODE_STRING GuidString,
    OUT REFGUID Guid
);

#define HASH_STRING_ALGORITHM_DEFAULT   (0u)
#define HASH_STRING_ALGORITHM_X65599    (1u)
#define HASH_STRING_ALGORITHM_INVALID   (~0ull)

NATIVE_API
NTSTATUS
NTAPI
RtlHashUnicodeString(
    IN  PCUNICODE_STRING    String,
    IN  BOOLEAN             CaseInSensitive,
    IN  ULONG               HashAlgorithm,
    OUT PULONG              HashValue
);

NATIVE_API
VOID
NTAPI
RtlInitAnsiString(
    IN OUT  PANSI_STRING    DestinationString,
    IN      LPCSTR          SourceString
);

NATIVE_API
NTSTATUS
NTAPI
RtlInitAnsiStringEx(
    IN OUT  PANSI_STRING    DestinationString,
    IN      LPCSTR          SourceString
);

NATIVE_API
VOID
NTAPI
RtlInitUnicodeString(
    IN OUT  PUNICODE_STRING DestinationString,
    IN      LPCWSTR         SourceString
);

NATIVE_API
NTSTATUS
NTAPI
RtlInitUnicodeStringEx(
    IN OUT  PUNICODE_STRING DestinationString,
    IN      LPCWSTR         SourceString
);

NATIVE_API
NTSTATUS
NTAPI
RtlAnsiStringToUnicodeString(
    IN OUT  PUNICODE_STRING DestinationString,
    IN      PANSI_STRING    SourceString,
    IN      BOOLEAN         AllocateDestinationString
);

NATIVE_API
NTSTATUS
NTAPI
RtlUnicodeStringToAnsiString(
    IN OUT  PANSI_STRING    DestinationString,
    IN      PUNICODE_STRING SourceString,
    IN      BOOLEAN         AllocateDestinationString
);

NATIVE_API
VOID
NTAPI
RtlRunEncodeUnicodeString(
    IN OUT  PUCHAR          Seed,
    IN      PUNICODE_STRING String
);

NATIVE_API
VOID
NTAPI
RtlRunDecodeUnicodeString(
    IN UCHAR            Seed,
    IN PUNICODE_STRING  String
);


NATIVE_API ULONG   NlsAnsiCodePage;
NATIVE_API BOOLEAN NlsMbCodePageTag;

FORCEINLINE
VOID
RtlInitEmptyAnsiString(
    OUT PANSI_STRING    AnsiString,
    IN  PCSTR           Buffer,
    IN  ULONG_PTR       BufferSize
)
{
    AnsiString->Length        = 0;
    AnsiString->MaximumLength = (USHORT)BufferSize;
    AnsiString->Buffer        = (PSTR)Buffer;
}

FORCEINLINE
VOID
RtlInitEmptyUnicodeString(
    OUT PUNICODE_STRING UnicodeString,
    IN  PCWSTR          Buffer,
    IN  ULONG_PTR       BufferSize
)
{
    UnicodeString->Length        = 0;
    UnicodeString->MaximumLength = (USHORT)BufferSize;
    UnicodeString->Buffer        = (PWSTR)Buffer;
}

#endif // ML_KERNEL_MODE

#endif // _NTIFS_

#if CPP_DEFINED

EXTCPP
template<typename StringType>
inline
VOID
RtlInitEmptyString(
    OUT StringType*     String,
    IN  PCWSTR          Buffer = nullptr,
    IN  ULONG_PTR       BufferSize = 0
)
{
    String->Length        = 0;
    String->MaximumLength = (USHORT)BufferSize;
    String->Buffer        = (PWSTR)Buffer;
}

#endif // cpp

inline
NTSTATUS
RtlSetUnicodeString(
    PUNICODE_STRING UnicodeString,
    PCWSTR          String,
    ULONG_PTR       DEF_VAL(Length, -1)
)
{
    if (UnicodeString == nullptr)
        return STATUS_INVALID_PARAMETER;

    if (String == nullptr)
    {
        RtlFreeUnicodeString(UnicodeString);
        return STATUS_SUCCESS;
    }

    if (UnicodeString->Buffer == nullptr)
    {
        return RtlCreateUnicodeString(UnicodeString, String) ? STATUS_SUCCESS : STATUS_NO_MEMORY;
    }

    if (String == nullptr)
    {
        UnicodeString->Buffer[0]    = 0;
        UnicodeString->Length       = 0;

        return STATUS_SUCCESS;
    }

    if (Length == -1)
        Length = StrLengthW(String) * sizeof(WCHAR);

    if (Length < UnicodeString->MaximumLength)
    {
        UnicodeString->Length = (USHORT)Length;
        CopyMemory(UnicodeString->Buffer, String, Length);
        UnicodeString->Buffer[Length / sizeof(WCHAR)] = 0;

        return STATUS_SUCCESS;
    }

    RtlFreeUnicodeString(UnicodeString);
    return RtlCreateUnicodeString(UnicodeString, String) ? STATUS_SUCCESS : STATUS_NO_MEMORY;
}

#endif // _NTSTRING_H_9bfab99e_9722_48ad_9a23_53edcb64be76_