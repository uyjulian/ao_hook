#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Nls);

NTSTATUS
FASTCALL
AnsiToUnicode(
    PWCHAR      UnicodeBuffer,
    ULONG_PTR   BufferCount,
    LPCSTR      AnsiString,
    LONG_PTR    AnsiLength,
    PULONG_PTR  BytesInUnicode
)
{
    ULONG       Length;
    NTSTATUS    Status;

    if (AnsiLength == -1)
        AnsiLength = StrLengthA(AnsiString);

    Status = RtlMultiByteToUnicodeN(
                UnicodeBuffer,
                (ULONG)(BufferCount != -1 ? (BufferCount * sizeof(WCHAR)) : -1),
                &Length,
                (PCHAR)AnsiString,
                (ULONG)AnsiLength
             );
    if (!NT_SUCCESS(Status))
        return Status;

    if (Length / sizeof(WCHAR) < BufferCount)
        UnicodeBuffer[Length / sizeof(WCHAR)] = 0;

    if (BytesInUnicode != NULL)
        *BytesInUnicode = Length;

    return Status;
}

NTSTATUS
UnicodeToAnsi(
    PSTR        AnsiBuffer,
    ULONG_PTR   BufferCount,
    PCWSTR      UnicodeString,
    LONG_PTR    UnicodeLength,
    PULONG_PTR  BytesInAnsi
)
{
    ULONG       Length;
    NTSTATUS    Status;

    if (UnicodeLength == -1)
        UnicodeLength = StrLengthW(UnicodeString);

    Status = RtlUnicodeToMultiByteN(
                AnsiBuffer,
                (ULONG)BufferCount,
                &Length,
                (PWCHAR)UnicodeString,
                (ULONG)(UnicodeLength * sizeof(WCHAR))
             );

    if (!NT_SUCCESS(Status))
        return Status;

    if (Length < BufferCount)
        AnsiBuffer[Length] = 0;

    if (BytesInAnsi != NULL)
        *BytesInAnsi = Length;

    return Status;
}

NTSTATUS
AnsiToUnicodeString(
    PUNICODE_STRING Unicode,
    PCSTR           AnsiString,
    LONG_PTR        AnsiLength,
    BOOL            AllocateDestinationString
)
{
    ANSI_STRING Ansi;

    Ansi.Buffer         = (PSTR)AnsiString;
    Ansi.Length         = (USHORT)(AnsiLength == -1 ? StrLengthA(AnsiString) : AnsiLength);
    Ansi.MaximumLength  = Ansi.Length;

    return RtlAnsiStringToUnicodeString(Unicode, &Ansi, (BOOLEAN)AllocateDestinationString);
}

NTSTATUS
UnicodeToAnsiString(
    PANSI_STRING    Ansi,
    LPCWSTR         UnicodeString,
    LONG_PTR        UnicodeLength,
    BOOL            AllocateDestinationString
)
{
    UNICODE_STRING Unicode;

    Unicode.Buffer          = (PWSTR)UnicodeString;
    Unicode.Length          = (USHORT)((UnicodeLength == -1 ? StrLengthW(UnicodeString) : UnicodeLength) * sizeof(WCHAR));
    Unicode.MaximumLength   = Unicode.Length;

    return RtlUnicodeStringToAnsiString(Ansi, &Unicode, (BOOLEAN)AllocateDestinationString);
}

ML_NAMESPACE_END_(Nls);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
