#ifndef _MLNTGDI_H_27034c70_52be_440c_9442_2e83bff34107_
#define _MLNTGDI_H_27034c70_52be_440c_9442_2e83bff34107_


#if ML_USER_MODE

ML_NAMESPACE_BEGIN(Gdi);

typedef struct
{
    USHORT MajorVersion;
    USHORT MinorVersion;
    USHORT NumOfTables;
    USHORT SearchRange;
    USHORT EntrySelector;
    USHORT RangeShift;

} TT_OFFSET_TABLE;

// Tables in TTF file and there placement and name (tag)

typedef struct
{
    ULONG Tag;      // table name
    ULONG CheckSum; // Check sum
    ULONG Offset;   // Offset from beginning of file
    ULONG Length;   // length of the table in bytes

} TT_TABLE_DIRECTORY;

//Header of names table

typedef struct
{
    USHORT FormatSelector;    // format selector. Always 0
    USHORT NameRecordCount;   // Name Records count
    USHORT StorageOffset;     // Offset for strings storage, from start of the table

} TT_NAME_TABLE_HEADER, *PTT_NAME_TABLE_HEADER;

// Record in names table

enum
{
    TT_PLATFORM_ID_MAC_OS       = 0x0100,
    TT_PLATFORM_ID_WINDOWS      = 0x0300,

    TT_TABLE_TAG_NAME           = TAG4('name'),
    TT_TABLE_TAG_CMAP           = TAG4('cmap'),

    TT_ENCODEING_ID_ANSI        = 0x0000,
    TT_ENCODEING_ID_UTF16_BE    = 0x0100,

    TT_NAME_ID_FACENAME         = 0x0100,
    TT_NAME_ID_TYPE             = 0x0200,
    TT_NAME_ID_FOUNDER          = 0x0300,
    TT_NAME_ID_FULLNAME         = 0x0400,
    TT_NAME_ID_VERSION          = 0x0500,
};

typedef struct
{
    USHORT PlatformID;
    USHORT EncodingID;
    USHORT LanguageID;
    USHORT NameID;
    USHORT StringLength;
    USHORT StringOffset; // from start of storage area

} TT_NAME_RECORD, *PTT_NAME_RECORD;

typedef struct
{
    USHORT  Version;        // Table version number (0).
    USHORT	NumberOfTables; // Number of encoding tables, n.

} TT_CMAP_TABLE_HEADER, *PTT_CMAP_TABLE_HEADER;

typedef struct
{
    USHORT	PlatformID;                 // Platform ID.
    USHORT	EncodingID;                 // Platform-specific encoding ID.
    ULONG	DataOffset;                 // Byte offset from beginning of table to the subtable for this encoding.

} TT_CMAP_RECORD, *PTT_CMAP_RECORD;

#pragma pack()

ML_NAMESPACE_END_(Gdi);

#endif // r3

#endif // _MLNTGDI_H_27034c70_52be_440c_9442_2e83bff34107_