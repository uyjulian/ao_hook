#ifndef _PEIMAGE_H_52a4aaa3_0a38_4bb7_9a33_3072037589e2
#define _PEIMAGE_H_52a4aaa3_0a38_4bb7_9a33_3072037589e2


_ML_C_HEAD_

#define IMAGE_INVALID_ORDINAL   ((ULONG_PTR)(~0ull))

#define IMAGE_INVALID_RVA       ((ULONG_PTR)(~0ui64))
#define IMAGE_INVALID_VA        ((PVOID)(~0ui64))
#define IMAGE_INVALID_OFFSET    IMAGE_INVALID_RVA
#define IMAGE_MINIMUM_ADDRESS   (ULONG_PTR)0x10000

#define IMAGE_NTHEADERS(_ImageBase) ((PIMAGE_NT_HEADERS)PtrAdd((_ImageBase), (_ImageBase)->e_lfanew))

#define IMAGE_SECTION_BASE(_Section)    ((_Section)->VirtualAddress)
#define IMAGE_SECTION_SIZE(_Section)    ((_Section)->Misc.VirtualSize)
#define IMAGE_SECTION_TAIL(_Section)    (IMAGE_SECTION_BASE(_Section) + IMAGE_SECTION_SIZE(_Section))
#define IMAGE_SECTION_TAIL_ALIGN(_Section, _SectionAlignment) ROUND_UP(IMAGE_SECTION_TAIL(_Section), _SectionAlignment)

enum
{
    IMAGE_VALID_EXPORT_ADDRESS_TABLE    = 0x00000001,
    IMAGE_VALID_IMPORT_ADDRESS_TABLE    = 0x00000002,
    IMAGE_VALID_RESOURCE                = 0x00000004,
    IMAGE_VALID_RELOC                   = 0x00000008,
};

PVOID
GetImageBaseAddress(
    PVOID ImageAddress
);

BOOL
ValidateDataDirectory(
    PIMAGE_DATA_DIRECTORY   DataDirectory,
    ULONG_PTR               SizeOfImage
);

BOOL
IsValidImage(
    PVOID       ImageBase,
    ULONG_PTR   DEF_VAL(Flags, 0)
);

#if !defined(_MY_STATIC_LIB_)

EXTC IMAGE_DOS_HEADER __ImageBase;

#endif // _MY_STATIC_LIB_


#pragma pack(push, 1)

typedef struct
{
    BYTE    Width;          // Width, in pixels, of the image
    BYTE    Height;         // Height, in pixels, of the image
    BYTE    ColorCount;     // Number of colors in image (0 if >=8bpp)
    BYTE    Reserved;       // Reserved
    WORD    Planes;         // Color Planes
    WORD    BitCount;       // Bits per pixel
    DWORD   BytesInRes;     // how many bytes in this resource?
    WORD    ID;             // the ID

} GROUP_ICON_ENTRY, *PGROUP_ICON_ENTRY;

typedef struct
{
    WORD                Reserved;   // Reserved (must be 0)
    WORD                Type;       // Resource type (1 for icons)
    WORD                Count;      // How many images?
    GROUP_ICON_ENTRY    Entries[1]; // The entries for each image

} GROUP_ICON, *PGROUP_ICON;

#pragma pack(pop)

typedef struct _RELOCATE_ADDRESS_INFO
{
    PVOID AddressToRelocate;
    PVOID NewAddress;
    BOOL (*CanRelocate)(struct _RELOCATE_ADDRESS_INFO *Address, PVOID PointerToAddress);
    PVOID Context;

} RELOCATE_ADDRESS_INFO, *PRELOCATE_ADDRESS_INFO;

NTSTATUS
RelocateAddress(
    PLDR_MODULE             ImageModule,
    PRELOCATE_ADDRESS_INFO  Addresses,
    ULONG                   Count
);

ULONG_PTR
IATLookupRoutineRVAByEntry(
    PVOID ImageBase,
    PVOID RoutineEntry
);

PVOID
IATLookupRoutineByEntryNoFix(
    PVOID ImageBase,
    PVOID RoutineEntry
);

ULONG_PTR
IATLookupRoutineRVAByHashNoFix(
    PVOID       ImageBase,
    ULONG_PTR   Hash
);

PVOID
EATLookupRoutineByHashNoFix(
    PVOID       ImageBase,
    ULONG_PTR   Hash
);

PSTR
EATLookupNameByHashNoFix(
    PVOID       ImageBase,
    ULONG_PTR   Hash
);

PSTR
EATLookupNameByNameNoFix(
    PVOID   ImageBase,
    PSTR    Name
);

PVOID
FASTCALL
EATLookupRoutineByHashPNoFix(
    PVOID       ImageBase,
    ULONG_PTR   RoutineEntry
);

/************************************************************************
  x64 ver
************************************************************************/
PVOID
EATLookupRoutineByHashNoFix64(
    PVOID       ImageBase,
    ULONG_PTR   Hash
);

PVOID
EATLookupRoutineByHashPNoFix64(
    PVOID       ImageBase,
    ULONG_PTR   RoutineEntry
);

inline PVOID IATLookupRoutineByEntry(PVOID ImageBase, PVOID RoutineEntry)
{
    return IATLookupRoutineByEntryNoFix(GetImageBaseAddress(ImageBase), RoutineEntry);
}

inline ULONG_PTR IATLookupRoutineRVAByEntry(PVOID ImageBase, PVOID RoutineEntry)
{
    ImageBase = GetImageBaseAddress(ImageBase);
    if (ImageBase == nullptr)
        return IMAGE_INVALID_RVA;

    RoutineEntry = IATLookupRoutineByEntryNoFix(ImageBase, RoutineEntry);
    if (RoutineEntry == nullptr)
        return IMAGE_INVALID_RVA;

    return (ULONG_PTR)RoutineEntry - (ULONG_PTR)(ImageBase);
}

inline PVOID IATLookupRoutineByHash(PVOID ImageBase, ULONG_PTR Hash)
{
    ImageBase = GetImageBaseAddress(ImageBase);
    if (ImageBase == nullptr)
        return nullptr;

    Hash = IATLookupRoutineRVAByHashNoFix(ImageBase, Hash);

    return Hash == IMAGE_INVALID_RVA ? (PVOID)Hash : PtrAdd(ImageBase, Hash);
}

#define LOAD_MEM_DLL_INFO_MAGIC  TAG4('LMDI')

#define LMD_REMOVE_PE_HEADER        0x00000001
#define LMD_REMOVE_IAT              0x00000002
#define LMD_REMOVE_EAT              0x00000004
#define LMD_REMOVE_RES              0x00000008
#define LMD_IGNORE_IAT_DLL_MISSING  0x00000010
#define LMD_MAPPED_DLL              0x10000000

NTSTATUS
LoadDllFromMemory(
    PVOID           DllBuffer,
    ULONG           DllBufferSize,
    PUNICODE_STRING ModuleFileName,
    PVOID*          ModuleHandle    = nullptr,
    ULONG           Flags           = 0
);

NTSTATUS
UnmapDllSection(
    PVOID BaseAddress
);

_ML_C_TAIL_


#if CPP_DEFINED

PVOID LookupImportTable(PVOID ImageBase, PCSTR DllName, PCSTR RoutineName);
PVOID LookupImportTable(PVOID ImageBase, PCSTR DllName, ULONG Hash);

PVOID LookupExportTable(PVOID ImageBase, ULONG Hash);
PVOID LookupExportTable(PVOID ImageBase, PCSTR RoutineName);

inline PIMAGE_NT_HEADERS ImageNtHeadersFast(PVOID ImageBase, PULONG_PTR NtHeadersVersion = nullptr)
{
    PIMAGE_NT_HEADERS32 NtHeaders32;

    NtHeaders32 = (PIMAGE_NT_HEADERS32)PtrAdd(ImageBase, ((PIMAGE_DOS_HEADER)ImageBase)->e_lfanew);

    if (NtHeadersVersion != nullptr) switch (NtHeaders32->OptionalHeader.Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            *NtHeadersVersion = NtHeaders32->OptionalHeader.Magic;
            break;

        default:
            return nullptr;
    }

    return (PIMAGE_NT_HEADERS)NtHeaders32;
}

inline PIMAGE_NT_HEADERS ImageNtHeaders(PVOID ImageBase, PULONG_PTR NtHeadersVersion = nullptr)
{
    if (!IsValidImage(ImageBase))
        return nullptr;

    return ImageNtHeadersFast(ImageBase, NtHeadersVersion);
}

inline ULONG_PTR ImageGetSizeOfImage(PVOID ImageBase)
{
    ULONG_PTR Version;
    PIMAGE_NT_HEADERS NtHeaders;

    NtHeaders = ImageNtHeaders(ImageBase, &Version);

    switch (Version)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            return ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.SizeOfImage;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            return ((PIMAGE_NT_HEADERS32)NtHeaders)->OptionalHeader.SizeOfImage;
    }

    return 0;
}

typedef
NTSTATUS
(*WalkRelocTableCallback)(
    PVOID                           ImageBase,
    PIMAGE_BASE_RELOCATION2         RelocationEntry,
    PIMAGE_RELOCATION_ADDRESS_ENTRY Offset,
    PVOID                           Context
);

#define WalkRelocCallbackM(ImageBase, RelocationEntry, Offset, Context) [&] (PVOID ImageBase, PIMAGE_BASE_RELOCATION2 RelocationEntry, PIMAGE_RELOCATION_ADDRESS_ENTRY Offset, PVOID Context) -> NTSTATUS

template<class CallbackRoutine>
inline NTSTATUS WalkRelocTableT(PVOID ImageBase, CallbackRoutine Callback, PVOID Context = nullptr)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS32         NtHeaders32;
    PIMAGE_NT_HEADERS64         NtHeaders64;
    PIMAGE_DATA_DIRECTORY       RelocDirectory;
    PIMAGE_BASE_RELOCATION2     Relocation, RelocationEnd;
    ULONG_PTR                   SizeOfImage;

    DosHeader   = (PIMAGE_DOS_HEADER)ImageBase;
    NtHeaders32 = (PIMAGE_NT_HEADERS32)PtrAdd(DosHeader, DosHeader->e_lfanew);
    NtHeaders64 = nullptr;

    switch (NtHeaders32->OptionalHeader.Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            SizeOfImage     = NtHeaders32->OptionalHeader.SizeOfImage;
            RelocDirectory  = &NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            NtHeaders64     = (PIMAGE_NT_HEADERS64)NtHeaders32;
            SizeOfImage     = NtHeaders64->OptionalHeader.SizeOfImage;
            RelocDirectory  = &NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            break;

        default:
            return STATUS_INVALID_IMAGE_FORMAT;
    }

    if (!ValidateDataDirectory(RelocDirectory, NtHeaders32->OptionalHeader.SizeOfImage))
        return STATUS_INVALID_IMAGE_FORMAT;

    Relocation          = (PIMAGE_BASE_RELOCATION2)PtrAdd(ImageBase, RelocDirectory->VirtualAddress);
    RelocationEnd       = PtrAdd(Relocation, RelocDirectory->Size);

    while (Relocation < RelocationEnd)
    {
        ULONG_PTR   SizeOfBlock;
        PVOID       RelocateBase;
        NTSTATUS    Status;
        PIMAGE_RELOCATION_ADDRESS_ENTRY TypeOffset;

        TypeOffset      = Relocation->TypeOffset;
        SizeOfBlock     = Relocation->SizeOfBlock;
        RelocateBase    = PtrAdd(ImageBase, Relocation->VirtualAddress);

        if (SizeOfBlock < sizeof(*Relocation) - sizeof(Relocation->TypeOffset) ||
            SizeOfBlock >= SizeOfImage ||
            PtrAnd(RelocateBase, 0xFFF) != 0)
        {
            return STATUS_ILLEGAL_DLL_RELOCATION;
        }

        SizeOfBlock -= sizeof(*Relocation) - sizeof(Relocation->TypeOffset);

        for (; SizeOfBlock > 0; ++TypeOffset, SizeOfBlock -= sizeof(*TypeOffset))
        {
            Status = Callback(ImageBase, Relocation, TypeOffset, Context);
            if (!NT_SUCCESS(Status))
                return Status;
        }

        Relocation = PtrAdd(Relocation, Relocation->SizeOfBlock);
    }

    return STATUS_SUCCESS;
}

ForceInline NTSTATUS WalkRelocTableInternal(PVOID ImageBase, WalkRelocTableCallback Callback, PVOID Context)
{
    return WalkRelocTableT(ImageBase, Callback, Context);
}

template<class CallbackRoutine, class CallbackContext>
ForceInline NTSTATUS WalkRelocTable(PVOID ImageBase, CallbackRoutine Callback, CallbackContext Context)
{
    return WalkRelocTableInternal(ImageBase, (WalkRelocTableCallback)Callback, (PVOID)Context);
}

typedef struct
{
    PVOID                       ImageBase;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;

    union
    {
        PIMAGE_THUNK_DATA       ThunkData;
        PIMAGE_THUNK_DATA32     ThunkData32;
        PIMAGE_THUNK_DATA64     ThunkData64;
    };

    PCSTR                       DllName;
    PCSTR                       FunctionName;
    ULONG_PTR                   Ordinal;
    PVOID                       Context;

} WALK_IMPORT_TABLE_DATA, *PWALK_IMPORT_TABLE_DATA;

typedef NTSTATUS (*WalkImportTableCallback)(PWALK_IMPORT_TABLE_DATA Data);

#define WalkIATCallbackM(Data) [&] (PWALK_IMPORT_TABLE_DATA Data) -> NTSTATUS

typedef struct
{
    PVOID                       ImageBase;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;
    PVOID                       EndOfImage;
    PVOID                       EndOfTable;
    ULONG_PTR                   SizeOfImage;
    ULONG_PTR                   SizeOfTable;
    ULONG_PTR                   NtHeadersVersion;

} WALK_IMPORT_TABLE_INTERNAL_DATA, *PWALK_IMPORT_TABLE_INTERNAL_DATA;

template<class CallbackRoutine, class CallbackContext, class PIMAGE_THUNK_DATA_TYPE>
inline NTSTATUS WalkImportTableInternal(PWALK_IMPORT_TABLE_INTERNAL_DATA InternalData, CallbackRoutine Callback, CallbackContext Context)
{
    NTSTATUS                    Status;
    PIMAGE_THUNK_DATA_TYPE      OriginalThunk, FirstThunk;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;

    ImportDescriptor = InternalData->ImportDescriptor;

    for (; ImportDescriptor->Name != NULL && ImportDescriptor->FirstThunk != NULL; ++ImportDescriptor)
    {
        LONG_PTR DllName;

        if (ImportDescriptor->FirstThunk > InternalData->SizeOfImage)
            continue;

        if (*(PULONG_PTR)PtrAdd(InternalData->ImageBase, ImportDescriptor->FirstThunk) == NULL)
            continue;

        OriginalThunk = (PIMAGE_THUNK_DATA_TYPE)InternalData->ImageBase;
        if (ImportDescriptor->OriginalFirstThunk != NULL)
        {
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->OriginalFirstThunk);
        }
        else
        {
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->FirstThunk);
        }

        if (OriginalThunk >= InternalData->EndOfImage)
            continue;

        DllName = PtrAdd((LONG_PTR)InternalData->ImageBase, ImportDescriptor->Name);
        if ((PVOID)DllName >= InternalData->EndOfImage)
            continue;

        FirstThunk = (PIMAGE_THUNK_DATA_TYPE)PtrAdd(InternalData->ImageBase, ImportDescriptor->FirstThunk);
        while (OriginalThunk->u1.AddressOfData != NULL)
        {
            LONG_PTR    FunctionName;
            ULONG_PTR   Ordinal;

            FunctionName = (LONG_PTR)OriginalThunk->u1.AddressOfData;
            if (FunctionName < 0)
            {
                Ordinal = (USHORT)FunctionName;
                FunctionName = NULL;
            }
            else
            {
                Ordinal = IMAGE_INVALID_ORDINAL;
                FunctionName += (LONG_PTR)PtrAdd(InternalData->ImageBase, 2);
            }

            WALK_IMPORT_TABLE_DATA Data;

            Data.ImageBase          = InternalData->ImageBase;
            Data.ImportDescriptor   = ImportDescriptor;
            Data.ThunkData          = (PIMAGE_THUNK_DATA)FirstThunk;
            Data.DllName            = (PCSTR)DllName;
            Data.Ordinal            = Ordinal;
            Data.FunctionName       = (PCSTR)FunctionName;
            Data.Context            = (PVOID)(ULONG_PTR)Context;

            Status = Callback(&Data);
            if (Status == STATUS_VALIDATE_CONTINUE)
                break;

            FAIL_RETURN(Status);

            ++OriginalThunk;
            ++FirstThunk;
        }
    }

    return STATUS_SUCCESS;
}

template<class CallbackRoutine, class CallbackContext>
inline NTSTATUS WalkImportTableT(PVOID ImageBase, CallbackRoutine Callback, CallbackContext Context = nullptr)
{
    ULONG_PTR                   NtHeadersVersion;
    NTSTATUS                    Status;
    PIMAGE_NT_HEADERS32         NtHeaders32;
    PIMAGE_NT_HEADERS64         NtHeaders64;

    WALK_IMPORT_TABLE_INTERNAL_DATA InternalData;

    if (!IsValidImage(ImageBase, IMAGE_VALID_IMPORT_ADDRESS_TABLE))
        return STATUS_INVALID_IMAGE_FORMAT;

    NtHeaders32 = (PIMAGE_NT_HEADERS32)ImageNtHeaders(ImageBase, &NtHeadersVersion);
    NtHeaders64 = (PIMAGE_NT_HEADERS64)NtHeaders32;

    InternalData.ImageBase = ImageBase;
    InternalData.ImportDescriptor = nullptr;

    switch (NtHeadersVersion)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            InternalData.ImportDescriptor = PtrAdd(InternalData.ImportDescriptor, NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
            InternalData.SizeOfTable = NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
            InternalData.SizeOfImage = NtHeaders32->OptionalHeader.SizeOfImage;
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            InternalData.ImportDescriptor = PtrAdd(InternalData.ImportDescriptor, NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
            InternalData.SizeOfTable = NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;
            InternalData.SizeOfImage = NtHeaders64->OptionalHeader.SizeOfImage;
            break;

        default:
            return STATUS_INVALID_IMAGE_FORMAT;
    }

    InternalData.ImportDescriptor = PtrAdd(InternalData.ImportDescriptor, ImageBase);
    InternalData.EndOfImage = PtrAdd(ImageBase, InternalData.SizeOfImage);
    InternalData.EndOfTable = PtrAdd(InternalData.ImportDescriptor, InternalData.SizeOfTable);

    switch (NtHeadersVersion)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            return WalkImportTableInternal<CallbackRoutine, CallbackContext, PIMAGE_THUNK_DATA32>(&InternalData, Callback, Context);

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            return WalkImportTableInternal<CallbackRoutine, CallbackContext, PIMAGE_THUNK_DATA64>(&InternalData, Callback, Context);
    }

    return STATUS_INVALID_IMAGE_FORMAT;
}

typedef struct
{
    PVOID       ImageBase;
    PULONG      AddressOfFunction;
    PCSTR       DllName;
    ULONG_PTR   Ordinal;
    PCSTR       FunctionName;
    PVOID       Context;
    BOOL        IsForward;

} WALK_EXPORT_TABLE_DATA, *PWALK_EXPORT_TABLE_DATA;

typedef NTSTATUS (*WalkExportTableCallback)(PWALK_EXPORT_TABLE_DATA Data);

#define WalkEATCallbackM(Data) [&] (PWALK_EXPORT_TABLE_DATA Data) -> NTSTATUS

template<class CallbackRoutine, class CallbackContext>
inline NTSTATUS WalkExportTableT(PVOID ImageBase, CallbackRoutine Callback, CallbackContext Context = nullptr)
{
    PCSTR                       DllName;
    ULONG_PTR                   Ordinal, OrdinalBase, SizeOfTable, SizeOfImage, NtHeadersVersion;
    NTSTATUS                    Status;
    PIMAGE_NT_HEADERS32         NtHeaders32;
    PIMAGE_NT_HEADERS64         NtHeaders64;
    PIMAGE_EXPORT_DIRECTORY     ExportDirectory;
    ULONG_PTR                   NumberOfNames, NumberOfFunctions, NumberOfOrdinals, MaskSize;
    PULONG                      AddressOfFuntions;
    PULONG                      WalkedMask;
    PUSHORT                     AddressOfNameOrdinals;
    PCSTR                      *AddressOfNames;
    PVOID                       EndOfImage, EndOfTable;

    if (!IsValidImage(ImageBase, IMAGE_VALID_EXPORT_ADDRESS_TABLE))
        return STATUS_INVALID_IMAGE_FORMAT;

    NtHeaders32 = (PIMAGE_NT_HEADERS32)ImageNtHeaders(ImageBase, &NtHeadersVersion);
    NtHeaders64 = (PIMAGE_NT_HEADERS64)NtHeaders32;
    if (NtHeaders32 == nullptr)
        return STATUS_INVALID_IMAGE_FORMAT;

    ExportDirectory = nullptr;
    ExportDirectory = PtrAdd(ExportDirectory, ImageBase);

    switch (NtHeadersVersion)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            ExportDirectory = PtrAdd(ExportDirectory, NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
            SizeOfTable = NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
            SizeOfImage = NtHeaders32->OptionalHeader.SizeOfImage;
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            ExportDirectory = PtrAdd(ExportDirectory, NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
            SizeOfTable = NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
            SizeOfImage = NtHeaders64->OptionalHeader.SizeOfImage;
            break;

        default:
            return STATUS_INVALID_IMAGE_FORMAT;
    }

    EndOfTable              = PtrAdd(ExportDirectory, SizeOfTable);
    EndOfImage              = PtrAdd(ImageBase, SizeOfImage);
    NumberOfNames           = ExportDirectory->NumberOfNames;
    NumberOfFunctions       = ExportDirectory->NumberOfFunctions;
    NumberOfOrdinals        = NumberOfFunctions - NumberOfNames;
    AddressOfFuntions       = PtrAdd((PULONG)ImageBase, ExportDirectory->AddressOfFunctions);
    AddressOfNames          = PtrAdd((PCSTR *)ImageBase, ExportDirectory->AddressOfNames);
    AddressOfNameOrdinals   = PtrAdd((PUSHORT)ImageBase, ExportDirectory->AddressOfNameOrdinals);
    DllName                 = (PCSTR)ImageBase + ExportDirectory->Name;

    if (AddressOfFuntions >= EndOfImage)
    {
        return STATUS_INVALID_IMAGE_WIN_32;
    }

    AddressOfNames = AddressOfNames >= EndOfImage ? nullptr : AddressOfNames;
    AddressOfNameOrdinals = AddressOfNameOrdinals >= EndOfImage ? nullptr : AddressOfNameOrdinals;

    OrdinalBase = ExportDirectory->Base;

    MaskSize = ROUND_UP((NumberOfFunctions + OrdinalBase + 1), bitsof(*WalkedMask)) / bitsof(*WalkedMask);
    MaskSize = MaskSize * sizeof(*WalkedMask);
    WalkedMask = (PULONG)AllocStack(MaskSize);
    if (WalkedMask == nullptr)
        return STATUS_NO_MEMORY;

    ZeroMemory(WalkedMask, MaskSize);

    if (AddressOfNameOrdinals != nullptr && AddressOfNames != nullptr)
    for (; NumberOfNames; ++AddressOfNames, ++AddressOfNameOrdinals, --NumberOfNames)
    {
        PCSTR       FunctionName;
        ULONG_PTR   Index, Mask;
        PULONG      Function;

        FunctionName = PtrAdd((PCSTR)ImageBase, *AddressOfNames);
        Ordinal = *AddressOfNameOrdinals;

        if (Ordinal >= NumberOfFunctions)
            continue;

        Function = AddressOfFuntions + *AddressOfNameOrdinals;
        if (*Function >= SizeOfImage || *Function == NULL)
            continue;

        Ordinal += OrdinalBase;

        Index = Ordinal / bitsof(*WalkedMask);
        Mask = (ULONG_PTR)(1 << ((Ordinal + 1) % bitsof(*WalkedMask) - 1));
        WalkedMask[Index] |= Mask;

        WALK_EXPORT_TABLE_DATA Data;

        Data.ImageBase = ImageBase;
        Data.AddressOfFunction = Function;
        Data.DllName = DllName;
        Data.Ordinal = Ordinal;
        Data.FunctionName = FunctionName;
        Data.Context = (PVOID)(ULONG_PTR)Context;
        Data.IsForward = PtrAdd(ImageBase, *Function) >= ExportDirectory && PtrAdd(ImageBase, *Function) < EndOfTable;

        Status = Callback(&Data);
        if (!NT_SUCCESS(Status))
        {
            //FreeMemory(WalkedMask);
            return Status;
        }
    }

    Ordinal = OrdinalBase;

    for (; NumberOfFunctions; ++AddressOfFuntions, ++Ordinal, --NumberOfFunctions)
    {
        if (FLAG_ON(WalkedMask[Ordinal / bitsof(*WalkedMask)], 1 << ((Ordinal + 1) % bitsof(*WalkedMask) - 1)))
        {
            continue;
        }

        if (*AddressOfFuntions == NULL)
            continue;

        WALK_EXPORT_TABLE_DATA Data;

        Data.ImageBase = ImageBase;
        Data.AddressOfFunction = AddressOfFuntions;
        Data.DllName = DllName;
        Data.Ordinal = Ordinal;
        Data.FunctionName = nullptr;
        Data.Context = (PVOID)(ULONG_PTR)Context;
        Data.IsForward = PtrAdd(ImageBase, *AddressOfFuntions) >= ExportDirectory && PtrAdd(ImageBase, *AddressOfFuntions) < EndOfTable;

        Status = Callback(&Data);
        if (!NT_SUCCESS(Status))
        {
            //FreeMemory(WalkedMask);
            return Status;
        }
    }

    //FreeMemory(WalkedMask);
    return STATUS_SUCCESS;
}

ForceInline NTSTATUS WalkExportTableInternal(PVOID ImageBase, WalkExportTableCallback Callback, PVOID Context)
{
    return WalkExportTableT(ImageBase, Callback, Context);
}

template<class CallbackRoutine, class CallbackContext>
ForceInline NTSTATUS WalkExportTable(PVOID ImageBase, CallbackRoutine Callback, CallbackContext Context)
{
    return WalkExportTableInternal(ImageBase, (WalkExportTableCallback)Callback, (PVOID)Context);
}

#define WalkDITCallbackM(Data) [&] (PWALK_IMPORT_TABLE_DATA Data) -> NTSTATUS

template<class CallbackRoutine, class CallbackContext>
inline NTSTATUS WalkDelayImportT(PVOID ImageBase, CallbackRoutine Callback, CallbackContext Context = nullptr)
{
    ULONG_PTR                   NtHeadersVersion;
    NTSTATUS                    Status;
    PIMAGE_NT_HEADERS32         NtHeaders32;
    PIMAGE_NT_HEADERS64         NtHeaders64;

    WALK_IMPORT_TABLE_INTERNAL_DATA InternalData;

    if (!IsValidImage(ImageBase, IMAGE_VALID_IMPORT_ADDRESS_TABLE))
        return STATUS_INVALID_IMAGE_FORMAT;

    NtHeaders32 = (PIMAGE_NT_HEADERS32)ImageNtHeaders(ImageBase, &NtHeadersVersion);
    NtHeaders64 = (PIMAGE_NT_HEADERS64)NtHeaders32;

    InternalData.ImageBase = ImageBase;
    InternalData.ImportDescriptor = nullptr;

    switch (NtHeadersVersion)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            InternalData.ImportDescriptor = PtrAdd(InternalData.ImportDescriptor, NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress);
            InternalData.SizeOfTable = NtHeaders32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;
            InternalData.SizeOfImage = NtHeaders32->OptionalHeader.SizeOfImage;
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            InternalData.ImportDescriptor = PtrAdd(InternalData.ImportDescriptor, NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress);
            InternalData.SizeOfTable = NtHeaders64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size;
            InternalData.SizeOfImage = NtHeaders64->OptionalHeader.SizeOfImage;
            break;

        default:
            return STATUS_INVALID_IMAGE_FORMAT;
    }

    InternalData.ImportDescriptor = PtrAdd(InternalData.ImportDescriptor, ImageBase);
    InternalData.EndOfImage = PtrAdd(ImageBase, InternalData.SizeOfImage);
    InternalData.EndOfTable = PtrAdd(InternalData.ImportDescriptor, InternalData.SizeOfTable);

    switch (NtHeadersVersion)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            return WalkImportTableInternal<CallbackRoutine, CallbackContext, PIMAGE_THUNK_DATA32>(&InternalData, Callback, Context);

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            return WalkImportTableInternal<CallbackRoutine, CallbackContext, PIMAGE_THUNK_DATA64>(&InternalData, Callback, Context);
    }

    return STATUS_INVALID_IMAGE_FORMAT;
}

#define WalkOpCodeM(Buffer, OpLength, Ret) \
            [&] (PBYTE Buffer, ULONG_PTR OpLength, PVOID &Ret) -> NTSTATUS

template<typename T>
PVOID WalkOpCode64T(PVOID Buffer, LONG_PTR Size, T Callback)
{
    PVOID ret = nullptr;

    PBYTE _Buffer = (PBYTE)Buffer;

    for (; Size > 0; )
    {
        ULONG_PTR Length;

        Length = GetOpCodeSize64(_Buffer);

        if (NT_SUCCESS(Callback(_Buffer, Length, ret)))
            break;

        _Buffer += Length;
        Size -= Length;
    }

    return ret;
}

template<typename T>
PVOID WalkOpCodeT(PVOID Buffer, LONG_PTR Size, T Callback)
{
    PVOID ret = nullptr;

    PBYTE _Buffer = (PBYTE)Buffer;

    for (; Size > 0; )
    {
        ULONG_PTR Length;

        Length = GetOpCodeSize(_Buffer);

        if (NT_SUCCESS(Callback(_Buffer, Length, ret)))
            break;

        _Buffer += Length;
        Size -= Length;
    }

    return ret;
}

#endif // cpp

#endif // _PEIMAGE_H_52a4aaa3_0a38_4bb7_9a33_3072037589e2