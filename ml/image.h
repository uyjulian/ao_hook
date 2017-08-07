#ifndef _IMAGE_H_e93dede9_b8fd_4157_ae1a_b888658c3ce8
#define _IMAGE_H_e93dede9_b8fd_4157_ae1a_b888658c3ce8


#if MY_OS_WIN32

// #if !ML_AMD64
//     #pragma INCLUDE_LIB(ucidec.lib)
// #endif

    EXTC_IMPORT int STDCALL UCIDecode(const void *src, int srclen, void** dst, int* stride, int* w, int* h, int* bpp);
    EXTC_IMPORT int STDCALL UCIFree(void* p);

#endif

#if CPP_DEFINED

#if MY_OS_WIN32

#define RGBA_GetRValue(color) ((Byte)((color) >> 0))
#define RGBA_GetGValue(color) ((Byte)((color) >> 8))
#define RGBA_GetBValue(color) ((Byte)((color) >> 16))
#define RGBA_GetAValue(color) ((Byte)((color) >> 24))
#define RGBA(r, g, b, a) (UInt32)(((u32)(u8)(r)) | (((u32)(u8)(g) << 8)) | (((u32)(u8)(b) << 16)) | (((u32)(u8)(a) << 24)))

#endif // MY_OS_WIN32

#pragma pack(push, 1)

typedef struct
{
    UInt16 Tag;                   // 0x00
    UInt32 FileSize;             // 0x02
    UInt32 Reserve;              // 0x06
    UInt32 RawOffset;            // 0x0A
    struct
    {
        UInt32 InfoHeaderSize;   // 0x0E
        Int32  Width;            // 0x12
        Int32  Height;           // 0x16
        UInt16 Layer;             // 0x1A
        UInt16 Bit;               // 0x1C
        UInt32 Compressed;       // 0x1E
        UInt32 SizeImage;        // 0x22
        Int32  XPelsPerMeter;     // 0x26
        Int32  YPelsPerMeter;     // 0x2A
        UInt32 ClrUsed;          // 0x2E
        UInt32 ClrImportant;     // 0x32
    } Info;
} IMAGE_BITMAP_HEADER, *PIMAGE_BITMAP_HEADER;

enum
{
    TgaImageRga = 2,
};

typedef struct
{
    UChar  ID;
    UChar  PaletteType;
    UChar  ImageType;
    UInt16 PaletteEntry;
    UInt16 PaletteLength;
    UChar  PaletteBitCount;
    UInt16 OriginX;
    UInt16 OriginY;
    UInt16 Width;
    UInt16 Height;
    UChar  PixelDepth;
    UChar  ImageDescription;

} IMAGE_TGA_HEADER, *PIMAGE_TGA_HEADER;

typedef struct
{
    union
    {
        PVoid lpBuffer;
        PByte  pbBuffer;
    };
    Int32   Width;
    Int32   Height;
    Int32   BitsPerPixel;
    Int32   Stride;
    PVoid   ExtraInfo;
    UInt32  ExtraInfoSize;

} UCI_INFO;

#define UCI_META_INFO_MAGIC TAG4('META')

typedef struct
{
    UInt32 Magic;   // TAG4('META')
    UInt32 cbSize;
    Int32  Width;
    Int32  Height;
    Int32  BitsPerPixel;
} UCI_META_INFO;

typedef struct
{
    UCHAR   Width;
    UCHAR   Height;
    UCHAR   NumberOfColorInPalette;
    UCHAR   Reserve;

    union
    {
        struct
        {
            USHORT  Planes;
            USHORT  BitsPerPixel;
        } Ico;

        struct
        {
            USHORT  X;
            USHORT  Y;
        } Cur;
    };

    ULONG ImageSize;
    ULONG ImageOffset;

} IMAGE_ICO_IMAGE_ENTRY, *PIMAGE_ICO_IMAGE_ENTRY;

typedef struct
{
    USHORT Reserve;
    USHORT FileType;   // 1
    USHORT NumberOfImage;

    IMAGE_ICO_IMAGE_ENTRY   Entry[1];

} IMAGE_ICO_HEADER, *PIMAGE_ICO_HEADER;

#pragma pack(pop)

ForceInline Long GetBitmapStride(Long Width, Long BitsPerPixel)
{
    return (Width * BitsPerPixel / 8 + 3) & ~3;
}

inline
Bool
FASTCALL
InitBitmapHeader(
    IMAGE_BITMAP_HEADER*Header,
    Int32               Width,
    Int32               Height,
    Int32               BitsPerPixel,
    PLongPtr            Stride
)
{
    UInt32 LocalStride, PaletteSize;

    if (Header == NULL)
        return False;

    ZeroMemory(Header, sizeof(*Header));

    PaletteSize = BitsPerPixel == 8 ? 256 * 4 : 0;

    Header->RawOffset = sizeof(*Header) + PaletteSize;
    Header->Info.Height = Height;
    Header->Info.Width  = Width;
    Header->Tag = TAG2('BM');
    Header->Info.InfoHeaderSize = sizeof(Header->Info);
    Header->Info.Layer = 1;
    Header->Info.Bit = (UInt16)BitsPerPixel;
//    pHeader->Info.dwClrUsed = 1;
    LocalStride = (Width * BitsPerPixel / 8 + 3) & ~3;
    if (Stride)
        *Stride = LocalStride;

    Header->FileSize = Height * LocalStride + sizeof(*Header) + PaletteSize;

    return True;
}

#if MY_OS_WIN32

inline PVoid SkipUCIStream(PVoid Data, Int32 DataSize, PUInt32 ExtraInfoSize)
{
    PByte Buffer = (PByte)Data;
    UInt32 StreamCount;

    switch (*(PUInt32)Buffer)
    {
        case TAG4('UCI3'):
        case TAG4('UCI '):
        case TAG4('UCI@'): StreamCount = 1; break;
        case TAG4('UCI4'):
        case TAG4('UCI!'):
        case TAG4('UCIA'): StreamCount = 2; break;
        case TAG4('UCIT'): StreamCount = 3; break;
        case TAG4('UCIQ'): StreamCount = 4; break;
        default: return NULL;
    }

    Buffer += 0xC;
    DataSize -= 0xC;
    while (StreamCount--)
    {
        UInt32 Size = *(PUInt32)Buffer + 4;
        Buffer += Size;
        DataSize -= Size;
    }

    if (DataSize <= 0)
    {
        if (ExtraInfoSize)
           *ExtraInfoSize = 0;
        return NULL;
    }

    if (ExtraInfoSize)
        *ExtraInfoSize = DataSize;

    return Buffer;
}

inline Int32 UCIDecodeEx(PVoid Data, Int32 DataSize, UCI_INFO *UCIInfo, Bool GetExtraInfo)
{
    Int32 ret;

    if (UCIInfo == NULL)
        return -1;

    ret = UCIDecode(Data, DataSize, &UCIInfo->lpBuffer, &UCIInfo->Stride, &UCIInfo->Width, &UCIInfo->Height, &UCIInfo->BitsPerPixel);
    if (ret < 0)
        return ret;

    if (GetExtraInfo)
    {
        UCIInfo->ExtraInfo = SkipUCIStream(Data, DataSize, &UCIInfo->ExtraInfoSize);
    }
    else
    {
        UCIInfo->ExtraInfo = NULL;
        UCIInfo->ExtraInfoSize = 0;
    }

    return ret;
}

inline Void UCIFreeEx(UCI_INFO *UCIInfo)
{
    if (UCIInfo == NULL)
        return;

    UCIFree(UCIInfo->lpBuffer);
}

#endif // MY_OS_WIN32

inline
UInt32
ConvertRawTo8Bit(
    UCI_INFO   *UciInfo,
    Int32       Width,
    Int32       Height,
    Int32       BitsPerPixel,
    PVoid       pvBuffer,
    UInt32      BufferSize
)
{
    PByte pbSrc, pbDest;
    LongPtr BytesPerPixel, Stride;
    IMAGE_BITMAP_HEADER *pBmp;

    UNREFERENCED_PARAMETER(Width);
    UNREFERENCED_PARAMETER(Height);
    UNREFERENCED_PARAMETER(BitsPerPixel);

    if (BufferSize < sizeof(*pBmp))
        return 0;

    pBmp = (IMAGE_BITMAP_HEADER *)pvBuffer;
    InitBitmapHeader(pBmp, Width, Height, BitsPerPixel, &Stride);
    if (pBmp->FileSize > BufferSize)
        return 0;

    pbDest = (PByte)(pBmp + 1);
    for (ULong Color = 0, Count = 256; Count; Color += 0x00010101, --Count)
    {
        *(PULong)pbDest = Color;
        pbDest += 4;
    }

    BytesPerPixel = UciInfo->BitsPerPixel / 8;
    pbSrc  = UciInfo->pbBuffer + (UciInfo->Height - 1) * UciInfo->Stride;
    pbDest = (PByte)pBmp + pBmp->RawOffset;
    for (ULong Height = UciInfo->Height; ;)
    {
        PByte p1, p2;

        p1 = pbSrc;
        p2 = pbDest;
        for (ULong Width = UciInfo->Width; ;)
        {
            ULong R, G, B, Color = *(PULong)p1;

            // Y = 0.299R + 0.587G + 0.114B

            /************************************************************************
              full range打算用这个常用的矩阵参数, JPEG应也用的是这个吧

                  Y = 0.11448 * B + 0.58661 * G + 0.29891 * R
                  U = 0.50000 * B - 0.33126 * G - 0.16874 * R + 128
                  V =-0.08131 * B - 0.41869 * G + 0.50000 * R + 128

               优化后的公式如下:

                  Y = (120041*B + 615105*G + 313430*R + 0x007FFFF) >> 20
                  U = (524288*B - 347351*G - 176937*R + 0x807FFFF) >> 20
                  V = (-85260*B - 439028*G + 524288*R + 0x807FFFF) >> 20
            ************************************************************************/

            R = RGBA_GetRValue(Color);
            G = RGBA_GetGValue(Color);
            B = RGBA_GetBValue(Color);
            *p2 = (BYTE)((R * 313430 + G * 615105 + B * 120041 + 0x007FFFF) >> 20);
            ++p2;
            p1 += BytesPerPixel;

            if (--Width == 0)
                break;
        }

        pbSrc  -= UciInfo->Stride;
        pbDest += Stride;

        if (--Height == 0)
            break;
    }

    return pBmp->FileSize;
}

inline
UInt32
ConvertRawTo24Or32(
    UCI_INFO   *pUciInfo,
    Int32       Width,
    Int32       Height,
    Int32       BitsPerPixel,
    PVoid       pvBuffer,
    UInt32      BufferSize
)
{
    PByte pbSrc, pbDest;
    LongPtr Stride;
    IMAGE_BITMAP_HEADER *pBmp;

    if (BufferSize < sizeof(*pBmp))
        return 0;

    pBmp = (IMAGE_BITMAP_HEADER *)pvBuffer;
    InitBitmapHeader(pBmp, Width, Height, BitsPerPixel, &Stride);
    if (pBmp->FileSize > BufferSize)
        return 0;

    pbSrc  = pUciInfo->pbBuffer;
    pbDest = (PByte)pBmp;
    pbDest += (Height - 1) * Stride + sizeof(*pBmp);

    for (ULong Height = pUciInfo->Height; Height; --Height)
    {
        CopyMemory(pbDest, pbSrc, Stride);
        pbDest -= Stride;
        pbSrc  += pUciInfo->Stride;
    }

    return pBmp->FileSize;
}

inline
UInt32
ConvertRawToBitMap(
    UCI_INFO   *pUciInfo,
    UInt32      Width,
    UInt32      Height,
    UInt32      BitsPerPixel,
    PVoid       pvBuffer,
    UInt32      BufferSize
)
{
    if (BitsPerPixel == 8)
    {
        return ConvertRawTo8Bit(pUciInfo, Width, Height, BitsPerPixel, pvBuffer, BufferSize);
    }
    else
    {
        switch (BitsPerPixel)
        {
            case 24:
            case 32:
                return ConvertRawTo24Or32(pUciInfo, Width, Height, BitsPerPixel, pvBuffer, BufferSize);
        }
    }

    return 0;
}

// #pragma warning(default:4201)

#endif // CPP_DEFINED

#endif // _IMAGE_H_e93dede9_b8fd_4157_ae1a_b888658c3ce8