#ifndef _MLNTNLS_H_6adf14d1_ea54_410e_a7e3_0da09303e8d0_
#define _MLNTNLS_H_6adf14d1_ea54_410e_a7e3_0da09303e8d0_


ML_NAMESPACE_BEGIN(Nls);

NTSTATUS
FASTCALL
AnsiToUnicode(
    PWSTR       UnicodeBuffer,
    ULONG_PTR   BufferCount,
    PCSTR       AnsiString,
    LONG_PTR    AnsiLength = -1,
    PULONG_PTR  BytesInUnicode = nullptr
);

NTSTATUS
UnicodeToAnsi(
    PSTR        AnsiBuffer,
    ULONG_PTR   BufferCount,
    PCWSTR      UnicodeString,
    LONG_PTR    UnicodeLength = -1,
    PULONG_PTR  BytesInAnsi = nullptr
);

NTSTATUS
AnsiToUnicodeString(
    PUNICODE_STRING Unicode,
    PCSTR           AnsiString,
    LONG_PTR        AnsiLength = -1,
    BOOL            AllocateDestinationString = TRUE
);

NTSTATUS
UnicodeToAnsiString(
    PANSI_STRING    Ansi,
    PCWSTR          UnicodeString,
    LONG_PTR        UnicodeLength = -1,
    BOOL            AllocateDestinationString = TRUE
);

ML_NAMESPACE_END_(Nls);

#endif // _MLNTNLS_H_6adf14d1_ea54_410e_a7e3_0da09303e8d0_