#ifndef _NTGDI_H_5cbb7a71_7f51_4a8a_bb90_038b762c3ed7_
#define _NTGDI_H_5cbb7a71_7f51_4a8a_bb90_038b762c3ed7_


_ML_C_HEAD_

#if ML_KERNEL_MODE

DECLARE_HANDLE(HDC);
DECLARE_HANDLE(HFONT);

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;

} POINT, *PPOINT, *LPPOINT;

typedef struct _GLYPHMETRICS {
    UINT    gmBlackBoxX;
    UINT    gmBlackBoxY;
    POINT   gmptGlyphOrigin;
    short   gmCellIncX;
    short   gmCellIncY;
} GLYPHMETRICS, *LPGLYPHMETRICS;

typedef struct _FIXED {
#ifndef _MAC
    WORD    fract;
    short   value;
#else
    short   value;
    WORD    fract;
#endif
} FIXED;

typedef struct _MAT2 {
    FIXED  eM11;
    FIXED  eM12;
    FIXED  eM21;
    FIXED  eM22;
} MAT2, *LPMAT2;

typedef struct ENUMLOGFONTEXDVW *PENUMLOGFONTEXDVW;

#endif // r0


inline
ULONG
NTAPI
NtGdiGetGlyphOutline(
    IN  HDC             hDc,
    IN  WCHAR           UnicodeChar,
    IN  ULONG           Format,
    OUT LPGLYPHMETRICS  GlyphMetrics,
    IN  ULONG           BufferSize,
    OUT PVOID           Buffer,
    IN  LPMAT2          Mat2,
    IN  BOOL            IgnoreRotation
)
{
    return 0;
}

inline
HFONT
NTAPI
NtGdiHfontCreate(
    IN  PENUMLOGFONTEXDVW   EnumLogFont,
    IN  ULONG               SizeOfEnumLogFont,
    IN  LONG                LogFontType,
    IN  LONG                Unknown,
    IN  PVOID               FreeListLocalFont
)
{
    return 0;
}


#if ML_USER_MODE

typedef struct
{
    ULONG SizeOfSelf;
    ULONG Unknown1;
    ULONG Unknown2;
    ULONG FontHandle;
    ULONG FontCount;
    ULONG Unknown5;

} FONT_REALIZATION_INFO, *PFONT_REALIZATION_INFO;

NATIVE_API
BOOL
NTAPI
GetFontRealizationInfo(
    HDC DC,
    PFONT_REALIZATION_INFO RealizationInfo
);

typedef struct
{
    ULONG Reserved[4];
    WCHAR FontFile[MAX_NTPATH];

} FONT_FILE_INFO, *PFONT_FILE_INFO;

NATIVE_API
BOOL
NTAPI
GetFontFileInfo(
    ULONG           FontHandle,
    ULONG           FontIndex,
    PFONT_FILE_INFO FontFileInfo,
    ULONG           InfoLength,
    PULONG          ReturnedLength
);

#endif // r3

_ML_C_TAIL_

#endif // _NTGDI_H_5cbb7a71_7f51_4a8a_bb90_038b762c3ed7_