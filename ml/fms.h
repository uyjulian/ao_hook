#ifndef _FMS_H_d46bc672_243a_4917_83c6_abbbbf435bb2_
#define _FMS_H_d46bc672_243a_4917_83c6_abbbbf435bb2_


#if ML_USER_MODE

DECLARE_HANDLE(FMS_ENUMERATOR);

typedef FMS_ENUMERATOR *PFMS_ENUMERATOR;


ML_NAMESPACE_BEGIN(FmsFilterType);

enum
{
    FaceName    = 0x02,
    CharSet     = 0x20,
};

ML_NAMESPACE_END_(FmsFilterType);

typedef struct
{
    ULONG   Unknown1;
    ULONG   FilterType;

    union
    {
        WCHAR   FaceName[1000];
        BYTE    Charset;
    };

} FMS_FILTER_DATA, *PFMS_FILTER_DATA;

typedef struct
{
    TEXTMETRICW TextMetric;
    ULONG       Unknown[10];

} FMS_TEXTMETRIC, *PFMS_TEXTMETRIC;

ML_NAMESPACE_BEGIN(FmsFontStyle);

enum
{
    Latin1      = 0,
    Latin2      = 1,
    Cyrillic    = 2,
    Thai        = 3,
    Jisjapan    = 4,
    Chinesesimp = 5,
    Latin2_2    = 6,
};

ML_NAMESPACE_END_(FmsFontStyle);

ML_NAMESPACE_BEGIN(FmsPropertyType);

enum
{
    FontTypeName        = 0x00,
    FontStyle           = 0x01,
    FaceNameLocale      = 0x02,
    FaceNameAnsi        = 0x03,
    FullNameLocale      = 0x04,
    FullNameAnsi        = 0x05,
    FontWeightName      = 0x06,
    FontStyleNameLocale = 0x09,
    FontStyleNameAnsi   = 0x0A,
    Company             = 0x13,
    FontFileName        = 0x17,
    FontDirectory       = 0x18,
    ScriptName          = 0x1F,
    FullDescription     = 0x16,
    SymbolInfo          = 0x25,

    PropertyMax         = 0x25,
};

ML_NAMESPACE_END_(FmsPropertyType);


ML_NAMESPACE_BEGIN(FmsInitializeFlags);

enum
{
    NoSimulations   = 0x00000001,
    NoVertFonts     = 0x00000004,
};

ML_NAMESPACE_END_(FmsInitializeFlags);

NATIVE_API
NTSTATUS
NTAPI
FmsInitializeEnumerator(
    IN  PFMS_ENUMERATOR Enumerator,
    IN  ULONG           Flags = FmsInitializeFlags::NoSimulations | FmsInitializeFlags::NoVertFonts
);

NATIVE_API
NTSTATUS
NTAPI
FmsResetEnumerator(
    IN FMS_ENUMERATOR Enumerator
);

NATIVE_API
NTSTATUS
NTAPI
FmsFreeEnumerator(
    IN OUT PFMS_ENUMERATOR Enumerator
);

NATIVE_API
NTSTATUS
NTAPI
FmsSetDefaultFilter(
    IN  FMS_ENUMERATOR Enumerator
);

NATIVE_API
NTSTATUS
NTAPI
FmsSetFilter(
    IN  FMS_ENUMERATOR      Enumerator,
    IN  PFMS_FILTER_DATA    FilterData,
    IN  ULONG               NumberOfFilterData
);

NATIVE_API
NTSTATUS
NTAPI
FmsAddFilter(
    IN  FMS_ENUMERATOR      Enumerator,
    IN  PFMS_FILTER_DATA    FilterData,
    IN  ULONG               NumberOfFilterData
);

NATIVE_API
NTSTATUS
NTAPI
FmsGetFilteredPropertyList(
    IN      FMS_ENUMERATOR  Enumerator,
    IN      ULONG           PropertyType,
    OUT     PULONG          NumberofProperty,
    IN OUT  PULONG          PropertySize,
    OUT     PVOID           Property    OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
FmsGetFilteredFontList(
    IN  FMS_ENUMERATOR  Enumerator,
    OUT PULONG          NumberOfFonts,
    OUT PULONG          FontIdList OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
FmsGetBestMatchInFamily(
    IN  FMS_ENUMERATOR  Enumerator,
    IN  ULONG           ReservedZero,
    IN  PCWSTR          FaceName,
    OUT PULONG          FontId
);

NATIVE_API
NTSTATUS
NTAPI
FmsGetGDILogFont(
    IN  FMS_ENUMERATOR      Enumerator,
    IN  ULONG               FontId,
    IN  BOOLEAN             WhatTrue,
    OUT LPENUMLOGFONTEXW    EnumLogFontEx,
    OUT PFMS_TEXTMETRIC     TextMetric OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
FmsGetGdiLogicalFont(
    IN  FMS_ENUMERATOR      Enumerator,
    IN  ULONG               FontId,
    IN  BOOLEAN             WhatTrue,
    OUT LPENUMLOGFONTEXW    EnumLogFontEx,
    OUT PFMS_TEXTMETRIC     FmsTextMetric  OPTIONAL,
    OUT PULONG              SomeBuffer  OPTIONAL
);

NATIVE_API
NTSTATUS
NTAPI
FmsMapGdiLogicalFont(
    IN  FMS_ENUMERATOR      Enumerator,
    IN  ULONG               FontId,
    OUT LPENUMLOGFONTEXW    EnumLogFontEx
);

NATIVE_API
NTSTATUS
NTAPI
FmsGetFontProperty(
    IN      FMS_ENUMERATOR  Enumerator,
    IN      ULONG           FontId,
    IN      ULONG           PropertyType,
    IN OUT  PULONG          PropertySize,
    IN OUT  PVOID           PropertyBuffer
);

#endif // r3

#endif // _FMS_H_d46bc672_243a_4917_83c6_abbbbf435bb2_