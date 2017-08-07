#ifndef _STRINGHELPER_H_4eabe4cb_a327_45e5_9b83_14061dceb3c6
#define _STRINGHELPER_H_4eabe4cb_a327_45e5_9b83_14061dceb3c6

//#include "Algorithm.h"
#include "ml/kmp.h"
#include <stdarg.h>

#pragma warning(disable:4127)

_ML_C_HEAD_

PChar                StringLowerA(PChar pszString, ULong DEF_VAL(Length, -1));
PWChar               StringLowerW(PWChar pszString, ULong DEF_VAL(Length, -1));
PChar                StringUpperA(PChar pszString, ULong DEF_VAL(Length, -1));
PWChar               StringUpperW(PWChar pszString, ULong DEF_VAL(Length, -1));
Long_Ptr    FASTCALL StrLengthA(PCChar pString);
Long_Ptr    FASTCALL SSE2_StrLengthA(PCChar pString);
Long_Ptr    FASTCALL StrLengthW(PCWChar pString);
PChar       FASTCALL StrCopyA(PChar DestinationString, PCChar SourceString);
PWChar      FASTCALL StrCopyW(PWChar DestinationString, PCWChar SourceString);
PChar       FASTCALL StrConcatenateA(PChar DestinationString, PCChar SourceString);
PWChar      FASTCALL StrConcatenateW(PWChar DestinationString, PCWChar SourceString);
Long_Ptr    FASTCALL StrNCompareA(PCChar pString1, PCChar pString2, SizeT LengthToCompare);

PChar   FASTCALL findexta(PCChar lpFileName);
PWChar  FASTCALL findextw(PCWChar lpFileName);
PChar   FASTCALL findnamea(PCChar lpFileName);
PWChar  FASTCALL findnamew(PCWChar lpFileName, SizeT DEF_VAL(Length, -1));
PWChar           StrFindCharW(PCWChar lpString, WChar CharMatch);
PWChar           PeekLineW(PVoid pBuffer, PVoid pEndOfBuffer, PVoid pOutBuffer, PSizeT pcbOutBuffer);

inline PTChar StrCopy(LPVoid DestinationString, LPCVoid SourceString)
{
    return sizeof(*(PTChar)SourceString) == sizeof(Char) ? (PTChar)StrCopyA((PChar)DestinationString, (PCChar)SourceString) : (PTChar)StrCopyW((PWChar)DestinationString, (PCWChar)SourceString);
}

inline PTChar findext(LPVoid lpFileName)
{
    return sizeof(*(PTChar)lpFileName) == sizeof(Char) ? (PTChar)findexta((PCChar)lpFileName) : (PTChar)findextw((PCWChar)lpFileName);
}

inline PTChar findname(PCTChar lpFileName)
{
    return sizeof(*(PTChar)lpFileName) == 1 ? (PTChar)findnamea((PCChar)lpFileName) : (PTChar)findnamew((PCWChar)lpFileName, ULONG_PTR_MAX);
}

inline PChar FASTCALL rmexta(PChar pszPath)
{
    *findexta(pszPath) = 0;
    return pszPath;
}

inline PWChar FASTCALL rmextw(PWChar pszPath)
{
    *findextw(pszPath) = 0;
    return pszPath;
}

inline PTChar FASTCALL rmext(PTChar pszPath)
{
    if (sizeof(*pszPath) == sizeof(Char))
        return (PTChar)rmexta((PChar)pszPath);
    else
        return (PTChar)rmextw((PWChar)pszPath);
}

inline Void FASTCALL chext(Char *szFileName, Char *pExt)
{
    StrCopyA(findexta(szFileName), pExt);
}

inline Void FASTCALL chextw(WChar *szFileName, WChar *pExt)
{
    StrCopyW(findextw(szFileName), pExt);
}

inline PChar FASTCALL rmname(PChar pszPath)
{
    *findnamea(pszPath) = 0;
    return pszPath;
}

inline PWChar FASTCALL rmnamew(PWChar pszPath)
{
    *findnamew(pszPath, ULONG_PTR_MAX) = 0;
    return pszPath;
}

inline Long_Ptr FASTCALL StrLength(PCTChar pString)
{
    if (sizeof(TChar) == sizeof(Char))
        return StrLengthA((PCChar)pString);
    else
        return StrLengthW((PCWChar)pString);
}

enum
{
    StrCmp_Keep,
    StrCmp_ToLower,
    StrCmp_ToUpper,
};

Long_Ptr FASTCALL StrNICompareA(PCChar pString1, PCChar pString2, SizeT LengthToCompare, LongPtr DEF_VAL(CaseConvertTo, StrCmp_Keep));

inline Long_Ptr FASTCALL StrICompareA(PCChar pString1, PCChar pString2, LongPtr DEF_VAL(CaseConvertTo, StrCmp_Keep))
{
    return StrNICompareA(pString1, pString2, (SizeT)~0ull, CaseConvertTo);
}

Long_Ptr FASTCALL StrNICompareW(PCWChar pString1, PCWChar pString2, SizeT LengthToCompare, LongPtr DEF_VAL(CaseConvertTo, StrCmp_Keep));

inline Long_Ptr FASTCALL StrICompareW(PCWChar pString1, PCWChar pString2, LongPtr DEF_VAL(CaseConvertTo, StrCmp_Keep))
{
    return StrNICompareW(pString1, pString2, (SizeT)-1, CaseConvertTo);
}

inline Long_Ptr FASTCALL StrNICompare(PCTChar pString1, PCTChar pString2, SizeT LengthToCompare, LongPtr DEF_VAL(CaseConvertTo, StrCmp_Keep))
{
    if (sizeof(*pString1) == sizeof(Char))
    {
        return StrNICompareA((PCChar)pString1, (PCChar)pString2, LengthToCompare, CaseConvertTo);
    }
    else if (sizeof(*pString1) == sizeof(WChar))
    {
        return StrNICompareW((PCWChar)pString1, (PCWChar)pString2, LengthToCompare, CaseConvertTo);
    }

    return 0;
}

inline Long_Ptr FASTCALL StrICompare(PCTChar pString1, PCTChar pString2)
{
    return StrNICompare(pString1, pString2, (SizeT)-1, StrCmp_Keep);
}

inline Long_Ptr FASTCALL StrCompareA(PCChar pString1, PCChar pString2)
{
    return StrNCompareA(pString1, pString2, (SizeT)-1);
}

Long_Ptr FASTCALL StrNCompareW(PCWChar pString1, PCWChar pString2, SizeT LengthToCompare);

inline Long_Ptr FASTCALL StrCompareW(PCWChar pString1, PCWChar pString2)
{
    return StrNCompareW(pString1, pString2, (SizeT)-1);
}

inline Long_Ptr FASTCALL StrNCompare(PCTChar pString1, PCTChar pString2, SizeT LengthToCompare)
{
    if (sizeof(*pString1) == sizeof(Char))
    {
        return StrNCompareA((PCChar)pString1, (PCChar)pString2, LengthToCompare);
    }
    else if (sizeof(*pString1) == sizeof(WChar))
    {
        return StrNCompareW((PCWChar)pString1, (PCWChar)pString2, LengthToCompare);
    }

    return 0;
}

inline Long_Ptr FASTCALL StrCompare(PCTChar pString1, PCTChar pString2)
{
    return StrNCompare(pString1, pString2, (SizeT)-1);
}

inline PChar SubStrA(const Char *_Src, const Char *_Sub)
{
    return (Char *)KMP(_Src, StrLengthA(_Src), _Sub, StrLengthA(_Sub));
}

inline PWChar SubStrW(const WChar *_Src, const WChar *_Sub)
{
    return (PWChar)KMP(_Src, StrLengthW(_Src) * sizeof(WChar), _Sub, StrLengthW(_Sub) * sizeof(WChar));
}



Int32 StringToInt32A(PCChar pszString);
Int32 StringToInt32W(PCWChar pszString);

inline Int32 StringToInt32(PCTChar pszString)
{
    if (sizeof(*pszString) == sizeof(Char))
        return StringToInt32A((PChar)pszString);
    else
        return StringToInt32W((PWChar)pszString);
}

Int32 StringToInt32HexW(PCWChar pszString);
Int64 StringToInt64HexW(PCWChar pszString);
Int64 StringToInt64A(PChar pszString);
Int64 StringToInt64W(PCWChar pszString);

inline Int64 StringToInt64(PTChar pszString)
{
    if (sizeof(TChar) == sizeof(Char))
        return StringToInt64A((PChar)pszString);
    else if (sizeof(TChar) == sizeof(WChar))
        return StringToInt64W((PWChar)pszString);

    return 0;
}

Bool IsShiftJISChar(SizeT uChar);
Bool IsShiftJISString(PCChar pString, SizeT Length);

inline int GetFormatedSize(PCWSTR Format, ...)
{
    return _vscwprintf(Format, (va_list)((PVOID *)&Format + 1));
}

Int FormatStringA  (PChar  pszBuffer, PCChar  pszFormat, ...);
Int FormatStringW  (PWChar pszBuffer, PCWChar pszFormat, ...);
Int FormatStringvA (PChar  pszBuffer, PCChar  pszFormat,  va_list args);
Int FormatStringvW (PWChar pszBuffer, PCWChar pszFormat,  va_list args);
Int FormatStringvnA(PChar  pszBuffer, UInt    cchLimitIn, PCChar  pszFormat, va_list args);
Int FormatStringvnW(PWChar pszBuffer, UInt    cchLimitIn, PCWChar pszFormat, va_list args);

// reactos\dll\win32\shlwapi\wsprintf.c

#define FMTSTR_FLAG_LEFTALIGN   0x00000001  /* Align output on the left ('-' prefix) */
#define FMTSTR_FLAG_PREFIX_HEX  0x00000002  /* Prefix hex with 0x ('#' prefix) */
#define FMTSTR_FLAG_ZEROPAD     0x00000004  /* Pad with zeros ('0' prefix) */
#define FMTSTR_FLAG_LONG        0x00000008  /* Long arg ('l' prefix) */
#define FMTSTR_FLAG_SHORT       0x00000010  /* Short arg ('h' prefix) */
#define FMTSTR_FLAG_UPPER_HEX   0x00000020  /* Upper-case hex ('X' specifier) */
#define FMTSTR_FLAG_WIDE        0x00000040  /* Wide arg ('w' prefix) */
#define FMTSTR_FLAG_FORCESIGNED 0x00000080  /* Force signed ('+' prefix) */
#define FMTSTR_FLAG_I64         0x00000100  /* 64 bit version ("%I64" prefix) */

typedef enum
{
    FMTSTR_FORMAT_UNKNOWN,
    FMTSTR_FORMAT_CHAR,
    FMTSTR_FORMAT_WCHAR,
    FMTSTR_FORMAT_STRING,
    FMTSTR_FORMAT_WSTRING,
    FMTSTR_FORMAT_SIGNED,
    FMTSTR_FORMAT_UNSIGNED,
    FMTSTR_FORMAT_HEXA,
    FMTSTR_FORMAT_FLOAT,

#if MY_OS_WIN32
    FMTSTR_FORMAT_BINARY,
#endif
} FMTSTR_TYPE;

typedef struct
{
    UInt        flags;
    UInt        width;
    UInt        precision;
    FMTSTR_TYPE type;
} FMTSTR_FORMAT;

typedef union
{
    WChar   wchar_view;
    Char    char_view;
    PCChar  lpcstr_view;
    PCWChar lpcwstr_view;
    Int     int_view;
    Int64   int64_view;
} FMTSTR_DATA;

Bool
StrMatchExpression(
    IN PWChar   Expression,
    IN PWChar   Name,
    IN Bool     DEF_VAL(IgnoreCase, TRUE),
    IN PWChar   DEF_VAL(UpcaseTable, NULL) OPTIONAL
);

_ML_C_TAIL_

#if CPP_DEFINED

#endif // CPP_DEFINED

#pragma warning(default:4127)

#endif // _STRINGHELPER_H_4eabe4cb_a327_45e5_9b83_14061dceb3c6