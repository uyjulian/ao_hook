#include "mylibrary.h"



#pragma warning(disable:4750)

_ML_C_HEAD_

#if ML_KERNEL_MODE

PVOID GetImageBaseAddress(PVOID ImageAddress)
{
    ULONG                           Size, BufferSize;
    NTSTATUS                        Status;
    PRTL_PROCESS_MODULE_INFORMATION Module;
    PRTL_PROCESS_MODULES            ModuleInformation;

    Size                = 0;
    BufferSize          = 0;
    ModuleInformation   = nullptr;

    LOOP_FOREVER
    {
        Status = ZwQuerySystemInformation(SystemModuleInformation, ModuleInformation, BufferSize, &Size);

        if (NT_SUCCESS(Status))
            break;

        if (Status != STATUS_INFO_LENGTH_MISMATCH)
            return nullptr;

        ModuleInformation = (PRTL_PROCESS_MODULES)AllocStack(Size - BufferSize);
        BufferSize = Size;
    }

    Module = ModuleInformation->Modules;
    for (ULONG Count = ModuleInformation->NumberOfModules; Count; --Count)
    {
        ULONG_PTR ImageBase, EndOfImage;

        ImageBase   = (ULONG_PTR)Module->ImageBase;
        EndOfImage  = ImageBase + Module->ImageSize;
        if (IN_RANGE(ImageBase, (ULONG_PTR)ImageAddress, EndOfImage))
            return (PVOID)ImageBase;

        ++Module;
    }

    return nullptr;
}

#else // user mode

PVOID GetImageBaseAddress(PVOID ImageAddress)
{
    PLDR_MODULE LdrModule;

    LdrModule = Ldr::FindLdrModuleByHandle(ImageAddress);

    return LdrModule == nullptr ? nullptr : LdrModule->DllBase;
}

#endif // ML_KERNEL_MODE

BOOL ValidateDataDirectory(PIMAGE_DATA_DIRECTORY DataDirectory, ULONG_PTR SizeOfImage)
{
    if (DataDirectory->Size == 0)
        return FALSE;

    if ((ULONG64)DataDirectory->VirtualAddress + DataDirectory->Size > ULONG_MAX)
        return FALSE;

    return DataDirectory->VirtualAddress <= SizeOfImage && DataDirectory->VirtualAddress + DataDirectory->Size <= SizeOfImage;
}

BOOL IsValidImage(PVOID ImageBase, ULONG_PTR Flags)
{
    PVOID                       Base, End, EndOfImage;
    ULONG_PTR                   Size, SizeOfImage;
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeader;
    PIMAGE_NT_HEADERS64         NtHeader64;
    PIMAGE_DATA_DIRECTORY       DataDirectory, Directory;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;

    DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    if (DosHeader->e_magic != IMAGE_DOS_SIGNATURE)
        return FALSE;

    NtHeader = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DosHeader->e_lfanew);
    if (NtHeader->Signature != IMAGE_NT_SIGNATURE)
        return FALSE;

    if (Flags == 0)
        return TRUE;

    switch (NtHeader->OptionalHeader.Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            DataDirectory = NtHeader->OptionalHeader.DataDirectory;
            SizeOfImage = NtHeader->OptionalHeader.SizeOfImage;
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            NtHeader64 = (PIMAGE_NT_HEADERS64)NtHeader;
            DataDirectory = NtHeader64->OptionalHeader.DataDirectory;
            SizeOfImage = NtHeader64->OptionalHeader.SizeOfImage;
            break;

        default:
            return FALSE;
    }

    if (FLAG_ON(Flags, IMAGE_VALID_IMPORT_ADDRESS_TABLE))
    {
        if (!ValidateDataDirectory(&DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT], SizeOfImage))
            return FALSE;

        ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)ImageBase + DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
        if (ImportDescriptor->Name == 0)
            return FALSE;
    }

    if (FLAG_ON(Flags, IMAGE_VALID_EXPORT_ADDRESS_TABLE))
    {
        PIMAGE_EXPORT_DIRECTORY ExportDirectory;

        if (!ValidateDataDirectory(&DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT], SizeOfImage))
            return FALSE;

        ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)((ULONG_PTR)ImageBase + DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
        if (ExportDirectory->AddressOfFunctions >= SizeOfImage)
            return FALSE;
    }

    if (FLAG_ON(Flags, IMAGE_VALID_RELOC))
    {
        if (!ValidateDataDirectory(&DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC], SizeOfImage))
            return FALSE;
    }

    if (FLAG_ON(Flags, IMAGE_VALID_RESOURCE))
    {
        if (!ValidateDataDirectory(&DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE], SizeOfImage))
            return FALSE;
    }

    return TRUE;
}

NTSTATUS
ReadRelocFromDisk(
    PWSTR       ImageModule,
    PVOID*      Relocation,
    PLONG_PTR   RelocSize
)
{
    PVOID                       RelocBuffer;
    NTSTATUS                    Status;
    NtFileDisk                  file;
    IMAGE_DOS_HEADER            DosHeader;
    IMAGE_NT_HEADERS32          NtHeaders32;
    IMAGE_NT_HEADERS64          NtHeaders64;
    PIMAGE_OPTIONAL_HEADER32    OptionalHeader32;
    PIMAGE_OPTIONAL_HEADER64    OptionalHeader64;
    PIMAGE_SECTION_HEADER       SectionHeader, Section;
    PIMAGE_DATA_DIRECTORY       RelocDirectory;

    *Relocation = nullptr;

    Status = file.Open(ImageModule);

    if (!NT_SUCCESS(Status))
        return Status;

    Status = file.Read(&DosHeader, sizeof(DosHeader));
    if (!NT_SUCCESS(Status))
        return Status;

    if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
        return STATUS_INVALID_IMAGE_WIN_32;

    Status = file.Seek(DosHeader.e_lfanew, FILE_BEGIN);
    if (!NT_SUCCESS(Status))
        return Status;

    Status = file.Read(&NtHeaders32, sizeof(NtHeaders32) - sizeof(NtHeaders32.OptionalHeader));
    if (!NT_SUCCESS(Status))
        return Status;

    if (NtHeaders32.Signature != IMAGE_NT_SIGNATURE)
        return STATUS_INVALID_IMAGE_WIN_32;

    OptionalHeader32 = (PIMAGE_OPTIONAL_HEADER32)AllocStack(NtHeaders32.FileHeader.SizeOfOptionalHeader);
    Status = file.Read(OptionalHeader32, NtHeaders32.FileHeader.SizeOfOptionalHeader);
    if (!NT_SUCCESS(Status))
        return Status;

    RelocDirectory = nullptr;
    switch (OptionalHeader32->Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            OptionalHeader64 = (PIMAGE_OPTIONAL_HEADER64)OptionalHeader32;
            RelocDirectory = &OptionalHeader64->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            break;

        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            RelocDirectory = &OptionalHeader32->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
            break;
    }

    if (RelocDirectory->Size == 0 || RelocDirectory->VirtualAddress == NULL)
        return STATUS_ILLEGAL_DLL_RELOCATION;

    SectionHeader = (PIMAGE_SECTION_HEADER)AllocStack(NtHeaders32.FileHeader.NumberOfSections * sizeof(*SectionHeader));
    Status = file.Read(SectionHeader, NtHeaders32.FileHeader.NumberOfSections * sizeof(*SectionHeader));
    if (!NT_SUCCESS(Status))
        return Status;

    ULONG_PTR SectionCount, RelocBegin, RelocEnd;

    RelocBegin  = RelocDirectory->VirtualAddress;
    RelocEnd    = RelocBegin + RelocDirectory->Size;

    Section = SectionHeader;
    for (SectionCount = NtHeaders32.FileHeader.NumberOfSections; SectionCount != 0; ++Section, --SectionCount)
    {
        if (Section->VirtualAddress > RelocEnd)
            continue;

        if (Section->VirtualAddress + Section->SizeOfRawData < RelocBegin)
            continue;

        break;
    }

    if (SectionCount == 0)
        return STATUS_ILLEGAL_DLL_RELOCATION;

    Status = file.Seek(Section->PointerToRawData + RelocDirectory->VirtualAddress - Section->VirtualAddress, FILE_BEGIN);
    if (!NT_SUCCESS(Status))
        return Status;

    RelocBuffer = AllocateMemoryP(RelocDirectory->Size);
    if (RelocBuffer == nullptr)
        return STATUS_NO_MEMORY;

    Status = file.Read(RelocBuffer, RelocDirectory->Size);
    if (!NT_SUCCESS(Status))
    {
        FreeMemoryP(RelocBuffer);
        return Status;
    }

    *Relocation = RelocBuffer;
    *RelocSize  = RelocDirectory->Size;

    return STATUS_SUCCESS;
}

NTSTATUS
ProcessOneRelocate(
    PLDR_MODULE             ImageModule,
    PIMAGE_BASE_RELOCATION2 RelocationBase,
    LONG_PTR                SizeOfRelocation,
    PRELOCATE_ADDRESS_INFO  Address
)
{
    PVOID       RelocateBase, ImageBase, ValuePtr, AddressToRelocate, NewAddress;
    LONG_PTR    SizeOfBlock;
    PIMAGE_BASE_RELOCATION2 Relocation;

    ImageBase           = ImageModule->DllBase;
    Relocation          = RelocationBase;
    AddressToRelocate   = Address->AddressToRelocate;
    NewAddress          = Address->NewAddress;

    for (; SizeOfRelocation > 0; )
    {
        PIMAGE_RELOCATION_ADDRESS_ENTRY TypeOffset;

        TypeOffset      = Relocation->TypeOffset;
        SizeOfBlock     = Relocation->SizeOfBlock;
        RelocateBase    = PtrAdd(ImageBase, Relocation->VirtualAddress);

        SizeOfRelocation    -= SizeOfBlock;
        SizeOfBlock         -= sizeof(*Relocation) - sizeof(Relocation->TypeOffset);

        for (; SizeOfBlock > 0; ++TypeOffset, SizeOfBlock -= sizeof(*TypeOffset))
        {
            if (*(PUSHORT)TypeOffset == 0)
                continue;

            ValuePtr = PtrAdd(RelocateBase, TypeOffset->Offset);

#if ML_KERNEL_MODE
            if (!MmIsAddressValid(ValuePtr))
                continue;
#endif

            if (*(PVOID *)ValuePtr != AddressToRelocate)
                continue;

            if (Address->CanRelocate != nullptr)
            {
                if (!Address->CanRelocate(Address, ValuePtr))
                    continue;
            }

            {
                PVOID Address;
#if ML_USER_MODE
                Address = ValuePtr;

#elif ML_KERNEL_MODE
                HookProtector hp(DISPATCH_LEVEL, ValuePtr, sizeof(PVOID));
#endif

                switch (TypeOffset->Type)
                {
                    case IMAGE_REL_BASED_ABSOLUTE:
                    case IMAGE_REL_BASED_HIGH:
                    case IMAGE_REL_BASED_LOW:
                    case IMAGE_REL_BASED_HIGHLOW:
                        *(PVOID *)Address = NewAddress;
                        break;

                    default:
                        break;
                }
            }
        }

        Relocation = PtrAdd(Relocation, Relocation->SizeOfBlock);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RelocateAddress(
    PLDR_MODULE             ImageModule,
    PRELOCATE_ADDRESS_INFO  Address,
    ULONG                   Count
)
{
    NTSTATUS                Status;
    LONG_PTR                SizeOfRelocation;
    PIMAGE_BASE_RELOCATION2 RelocationBase;

    Status = ReadRelocFromDisk(ImageModule->FullDllName.Buffer, (PVOID *)&RelocationBase, &SizeOfRelocation);
    if (!NT_SUCCESS(Status))
        return Status;

    for (; Count; --Count)
    {
        if (Address->AddressToRelocate != nullptr)
            ProcessOneRelocate(ImageModule, RelocationBase, SizeOfRelocation, Address);

        ++Address;
    }

    MemoryAllocator::FreeMemory(RelocationBase);

    return STATUS_SUCCESS;
}

PVOID IATLookupRoutineByEntryNoFix(PVOID ImageBase, PVOID RoutineEntry)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeaders;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;
    PIMAGE_THUNK_DATA           ThunkData;

    if (!IsValidImage(ImageBase, IMAGE_VALID_IMPORT_ADDRESS_TABLE))
        return nullptr;

    DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    NtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DosHeader->e_lfanew);

    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)ImageBase + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
    while (ImportDescriptor->Name)
    {
        ThunkData = (PIMAGE_THUNK_DATA)((ULONG_PTR)ImageBase + ImportDescriptor->FirstThunk);
        while (ThunkData->u1.Function)
        {
            if ((ULONG_PTR)ThunkData->u1.Function == (ULONG_PTR)RoutineEntry)
                return (PVOID)&ThunkData->u1.Function;
            ++ThunkData;
        }

        ++ImportDescriptor;
    }

    return nullptr;
}

ULONG_PTR IATLookupRoutineRVAByHashNoFix(PVOID ImageBase, ULONG_PTR Hash)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeaders;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;
    PIMAGE_THUNK_DATA           OriginalThunk, FirstThunk;

    if (IsValidImage(ImageBase, IMAGE_VALID_IMPORT_ADDRESS_TABLE) == FALSE)
        return IMAGE_INVALID_RVA;

    DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    NtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DosHeader->e_lfanew);

    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)ImageBase + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    for (; ImportDescriptor->Name != NULL && ImportDescriptor->FirstThunk != NULL; ++ImportDescriptor)
    {
        OriginalThunk = (PIMAGE_THUNK_DATA)ImageBase;
        if (ImportDescriptor->OriginalFirstThunk)
        {
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->OriginalFirstThunk);
        }
        else
        {
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->FirstThunk);
        }

        FirstThunk = (PIMAGE_THUNK_DATA)PtrAdd(ImageBase, ImportDescriptor->FirstThunk);

        for (; OriginalThunk->u1.AddressOfData != NULL; ++OriginalThunk, ++FirstThunk)
        {
            LONG_PTR FunctionName;

            FunctionName = OriginalThunk->u1.AddressOfData;
            if (FunctionName < 0)
                continue;

            FunctionName += (LONG_PTR)PtrAdd(ImageBase, 2);

            if ((HashAPI((PCSTR)FunctionName) ^ Hash) == 0)
            {
                return PtrOffset(&FirstThunk->u1.Function, ImageBase);
            }
        }
    }

    return IMAGE_INVALID_RVA;
}

PSTR EATLookupNameByHashNoFix(PVOID ImageBase, ULONG_PTR Hash)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeaders;
    PIMAGE_EXPORT_DIRECTORY     ExportDirectory;
    ULONG_PTR                   NumberOfNames;
    PULONG_PTR                  AddressOfFuntions;
    PCSTR                      *AddressOfNames;

    if (!IsValidImage(ImageBase, IMAGE_VALID_EXPORT_ADDRESS_TABLE))
        return nullptr;

    DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    NtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DosHeader->e_lfanew);

    ExportDirectory = nullptr;
    ExportDirectory = PtrAdd(ExportDirectory, ImageBase);
    ExportDirectory = PtrAdd(ExportDirectory, NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    NumberOfNames       = ExportDirectory->NumberOfNames;
    AddressOfFuntions   = PtrAdd((PULONG_PTR)ImageBase, ExportDirectory->AddressOfFunctions);
    AddressOfNames      = PtrAdd((PCSTR *)ImageBase, ExportDirectory->AddressOfNames);

    do
    {
        if (!(HashAPI(PtrAdd((PCSTR)(ImageBase), *AddressOfNames)) ^ Hash))
        {
            return PtrAdd((PSTR)(ImageBase), *AddressOfNames);
        }

        ++AddressOfNames;

    } while (--NumberOfNames);

    return nullptr;
}

PSTR EATLookupNameByNameNoFix(PVOID ImageBase, PSTR Name)
{
    return EATLookupNameByHashNoFix(ImageBase, HashAPI(Name));
}

PVOID EATLookupRoutineByHashNoFix(PVOID ImageBase, ULONG_PTR Hash)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeaders;
    PIMAGE_EXPORT_DIRECTORY     ExportDirectory;
    ULONG_PTR                   NumberOfNames;
    PULONG_PTR                  AddressOfFuntions;
    PCSTR                      *AddressOfNames;
    PUSHORT                     AddressOfNameOrdinals;

    if (Hash != 0 && !IsValidImage(ImageBase, IMAGE_VALID_EXPORT_ADDRESS_TABLE))
        return nullptr;

    DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    NtHeaders = (PIMAGE_NT_HEADERS)((ULONG_PTR)ImageBase + DosHeader->e_lfanew);

    switch (NtHeaders->OptionalHeader.Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            return EATLookupRoutineByHashNoFix64(ImageBase, Hash);
    }

    if (Hash == 0)
    {
        return &NtHeaders->OptionalHeader.AddressOfEntryPoint;
    }

    ExportDirectory = nullptr;
    ExportDirectory = PtrAdd(ExportDirectory, ImageBase);
    ExportDirectory = PtrAdd(ExportDirectory, NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    NumberOfNames           = ExportDirectory->NumberOfNames;
    AddressOfFuntions       = PtrAdd((PULONG_PTR)ImageBase, ExportDirectory->AddressOfFunctions);
    AddressOfNames          = PtrAdd((PCSTR *)ImageBase, ExportDirectory->AddressOfNames);
    AddressOfNameOrdinals   = PtrAdd((PUSHORT)ImageBase, ExportDirectory->AddressOfNameOrdinals);

    do
    {
        if (!(HashAPI(PtrAdd((PCSTR)(ImageBase), *AddressOfNames)) ^ Hash))
        {
            return AddressOfFuntions + *AddressOfNameOrdinals;
        }

        ++AddressOfNameOrdinals;
        ++AddressOfNames;
    } while (--NumberOfNames);

    return nullptr;
}

PVOID FASTCALL EATLookupRoutineByHashPNoFix(PVOID ImageBase, ULONG_PTR Hash)
{
    PVOID Pointer;

    Pointer = EATLookupRoutineByHashNoFix(ImageBase, Hash);
    if (Pointer != nullptr)
        Pointer = PtrAdd(ImageBase, *(PULONG)Pointer);

    return Pointer;
}

/************************************************************************
  x64 ver
************************************************************************/

PVOID EATLookupRoutineByHashNoFix64(PVOID ImageBase, ULONG_PTR Hash)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS64         NtHeaders;
    PIMAGE_EXPORT_DIRECTORY     ExportDirectory;
    ULONG_PTR                   NumberOfNames;
    PULONG                      AddressOfFuntions;
    PULONG                      AddressOfNames;
    PUSHORT                     AddressOfNameOrdinals;

    if (Hash != 0 && !IsValidImage(ImageBase, IMAGE_VALID_EXPORT_ADDRESS_TABLE))
        return nullptr;

    DosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    NtHeaders = (PIMAGE_NT_HEADERS64)((ULONG_PTR)ImageBase + DosHeader->e_lfanew);

    if (Hash == 0)
    {
        return &NtHeaders->OptionalHeader.AddressOfEntryPoint;
    }

    ExportDirectory = nullptr;
    ExportDirectory = PtrAdd(ExportDirectory, ImageBase);
    ExportDirectory = PtrAdd(ExportDirectory, NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    NumberOfNames           = ExportDirectory->NumberOfNames;
    AddressOfFuntions       = PtrAdd((PULONG)ImageBase, (ULONG_PTR)ExportDirectory->AddressOfFunctions);
    AddressOfNames          = PtrAdd((PULONG)ImageBase, (ULONG_PTR)ExportDirectory->AddressOfNames);
    AddressOfNameOrdinals   = PtrAdd((PUSHORT)ImageBase, (ULONG_PTR)ExportDirectory->AddressOfNameOrdinals);

    do
    {
        if (!(HashAPI(PtrAdd((PCSTR)(ImageBase), *AddressOfNames)) ^ Hash))
        {
            return AddressOfFuntions + *AddressOfNameOrdinals;
        }

        ++AddressOfNameOrdinals;
        ++AddressOfNames;
    } while (--NumberOfNames);

    return nullptr;
}

PVOID EATLookupRoutineByHashPNoFix64(PVOID ImageBase, ULONG_PTR Hash)
{
    PVOID Pointer;

    Pointer = EATLookupRoutineByHashNoFix64(ImageBase, Hash);
    if (Pointer != nullptr)
        Pointer = PtrAdd((PVOID)(ULONG_PTR)*(PULONG)Pointer, ImageBase);

    return Pointer;
}

#if CPP_DEFINED

_ML_CPP_HEAD_

PVOID LookupImportTable(PVOID ImageBase, PCSTR DllName, ULONG Hash)
{
    PCSTR TargetDllName;
    PVOID IATVA;

    IATVA = IMAGE_INVALID_VA;
    TargetDllName = nullptr;

    WalkImportTableT(ImageBase,
        WalkIATCallbackM(Data)
        {
            if (DllName != nullptr)
            {
                if (TargetDllName != nullptr)
                {
                    if (TargetDllName != Data->DllName)
                        return STATUS_NOT_FOUND;
                }
                else if (_stricmp(Data->DllName, DllName) != 0)
                {
                    return STATUS_VALIDATE_CONTINUE;
                }
                else
                {
                    TargetDllName = Data->DllName;
                }
            }

            if (Data->FunctionName == nullptr)
                return STATUS_SUCCESS;

            if (HashAPI(Data->FunctionName) == Hash)
            {
                IATVA = Data->ThunkData;
                return STATUS_NO_MORE_MATCHES;
            }

            return STATUS_SUCCESS;
        },
        0
    );

    return IATVA;
}

PVOID LookupImportTable(PVOID ImageBase, PCSTR DllName, PCSTR RoutineName)
{
    return LookupImportTable(ImageBase, DllName, HashAPI(RoutineName));
}

PVOID LookupExportTable(PVOID ImageBase, ULONG Hash)
{
    ULONG RoutineRVA = (ULONG)IMAGE_INVALID_RVA;

    WalkExportTableT(ImageBase,
        WalkEATCallbackM(Data)
        {
            if (Data->FunctionName == nullptr)
                return STATUS_NO_MORE_MATCHES;

            if (HashAPI(Data->FunctionName) == Hash)
            {
                RoutineRVA = *Data->AddressOfFunction;
                return STATUS_NO_MORE_MATCHES;
            }

            return STATUS_SUCCESS;
        },0
    );

    return RoutineRVA == IMAGE_INVALID_RVA ? IMAGE_INVALID_VA : PtrAdd(ImageBase, RoutineRVA);
}

PVOID LookupExportTable(PVOID ImageBase, PCSTR RoutineName)
{
    return LookupExportTable(ImageBase, HashAPI(RoutineName));
}

_ML_CPP_TAIL_

#endif // cpp

#if ML_USER_MODE

typedef struct LOAD_MEM_DLL_INFO : public TEB_ACTIVE_FRAME
{
    ULONG           Flags;
    PVOID           MappedBase;
    PVOID           MemDllBase;
    SIZE_T          DllBufferSize;
    SIZE_T          ViewSize;
    UNICODE_STRING  Lz32Path;

    union
    {
        HANDLE DllFileHandle;
        HANDLE SectionHandle;
    };

    UNICODE_STRING  MemDllFullPath;

    API_POINTER(NtQueryAttributesFile)  NtQueryAttributesFile;
    API_POINTER(NtOpenFile)             NtOpenFile;
    API_POINTER(NtCreateSection)        NtCreateSection;
    API_POINTER(NtMapViewOfSection)     NtMapViewOfSection;
    API_POINTER(NtClose)                NtClose;
    API_POINTER(NtQuerySection)         NtQuerySection;
    API_POINTER(LdrLoadDll)             LdrLoadDll;

} LOAD_MEM_DLL_INFO, *PLOAD_MEM_DLL_INFO;

PLOAD_MEM_DLL_INFO GetLaodMemDllInfo()
{
    return (PLOAD_MEM_DLL_INFO)FindThreadFrame(LOAD_MEM_DLL_INFO_MAGIC);
}

NTSTATUS
NTAPI
LoadMemoryDll_NtQueryAttributesFile(
    POBJECT_ATTRIBUTES      ObjectAttributes,
    PFILE_BASIC_INFORMATION FileInformation
)
{
    PLOAD_MEM_DLL_INFO MemDllInfo;

    MemDllInfo = GetLaodMemDllInfo();

    if (RtlCompareUnicodeString(ObjectAttributes->ObjectName, &MemDllInfo->MemDllFullPath, TRUE) != 0)
    {
        return MemDllInfo->NtQueryAttributesFile(ObjectAttributes, FileInformation);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
STDCALL
LoadMemoryDll_NtOpenFile(
    PHANDLE             FileHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PIO_STATUS_BLOCK    IoStatusBlock,
    ULONG               ShareAccess,
    ULONG               OpenOptions
)
{
    NTSTATUS            Status;
    PLOAD_MEM_DLL_INFO  MemDllInfo;

    MemDllInfo = GetLaodMemDllInfo();
    if (RtlCompareUnicodeString(ObjectAttributes->ObjectName, &MemDllInfo->MemDllFullPath, TRUE))
    {
        return MemDllInfo->NtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
    }

    ObjectAttributes->ObjectName = &MemDllInfo->Lz32Path;
    Status = MemDllInfo->NtOpenFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
    if (NT_SUCCESS(Status))
    {
        MemDllInfo->DllFileHandle = *FileHandle;
    }

    return Status;
}

NTSTATUS
STDCALL
LoadMemoryDll_NtCreateSection(
    PHANDLE             SectionHandle,
    ACCESS_MASK         DesiredAccess,
    POBJECT_ATTRIBUTES  ObjectAttributes,
    PLARGE_INTEGER      MaximumSize,
    ULONG               SectionPageProtection,
    ULONG               AllocationAttributes,
    HANDLE              FileHandle
)
{
    BOOL                IsDllHandle;
    NTSTATUS            Status;
    LARGE_INTEGER       SectionSize;
    PLOAD_MEM_DLL_INFO  MemDllInfo;

    IsDllHandle = FALSE;
    MemDllInfo = GetLaodMemDllInfo();

    if (FileHandle != nullptr)
    {
        if (MemDllInfo->DllFileHandle == FileHandle)
        {
//            if (MaximumSize == NULL)
            MaximumSize = &SectionSize;

            MaximumSize->QuadPart   = MemDllInfo->ViewSize;
            DesiredAccess           = SECTION_MAP_READ | SECTION_MAP_WRITE | SECTION_MAP_EXECUTE;
            SectionPageProtection   = PAGE_EXECUTE_READWRITE;
            AllocationAttributes    = SEC_COMMIT;
            FileHandle              = nullptr;
            IsDllHandle             = TRUE;
        }
    }

    Status = MemDllInfo->NtCreateSection(
                SectionHandle,
                DesiredAccess,
                ObjectAttributes,
                MaximumSize,
                SectionPageProtection,
                AllocationAttributes,
                FileHandle
            );

    if (!NT_SUCCESS(Status) || !IsDllHandle)
    {
        return Status;
    }

    MemDllInfo->SectionHandle = *SectionHandle;

    return Status;
}

NTSTATUS
STDCALL
LoadMemoryDll_NtMapViewOfSection(
    HANDLE              SectionHandle,
    HANDLE              ProcessHandle,
    PVOID              *BaseAddress,
    ULONG_PTR           ZeroBits,
    SIZE_T              CommitSize,
    PLARGE_INTEGER      SectionOffset,
    PSIZE_T             ViewSize,
    SECTION_INHERIT     InheritDisposition,
    ULONG               AllocationType,
    ULONG               Win32Protect
)
{
    NTSTATUS                    Status;
    PLOAD_MEM_DLL_INFO          MemDllInfo;
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeader;
    PIMAGE_SECTION_HEADER       SectionHeader;
    PBYTE                       DllBase, ModuleBase;

    MemDllInfo = GetLaodMemDllInfo();

    if (SectionHandle == nullptr)
        goto CALL_ORIGINAL;

    if (MemDllInfo == nullptr)
        goto CALL_ORIGINAL;

    if (SectionHandle != MemDllInfo->SectionHandle)
        goto CALL_ORIGINAL;

    if (SectionOffset != nullptr)
        SectionOffset->QuadPart = 0;

    *ViewSize = MemDllInfo->ViewSize;
    Status = MemDllInfo->NtMapViewOfSection(
                SectionHandle,
                ProcessHandle,
                BaseAddress,
                0,
                0,
                nullptr,
                ViewSize,
                ViewShare,
                0,
                PAGE_EXECUTE_READWRITE
             );
    if (NT_FAILED(Status))
        return Status;

    MemDllInfo->MappedBase = *BaseAddress;

    ModuleBase  = (PBYTE)*BaseAddress;
    DllBase     = (PBYTE)MemDllInfo->MemDllBase;
    DosHeader   = (PIMAGE_DOS_HEADER)DllBase;
    NtHeader    = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBase + DosHeader->e_lfanew);

    if (FLAG_ON(MemDllInfo->Flags, LMD_MAPPED_DLL))
    {
        CopyMemory(ModuleBase, DllBase, MemDllInfo->ViewSize);
    }
    else
    {
//        DosHeader       = (PIMAGE_DOS_HEADER)DllBase;
//        NtHeader        = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBase + DosHeader->e_lfanew);
        SectionHeader   = (PIMAGE_SECTION_HEADER)((ULONG_PTR)&NtHeader->OptionalHeader + NtHeader->FileHeader.SizeOfOptionalHeader);
        for (ULONG NumberOfSections = NtHeader->FileHeader.NumberOfSections; NumberOfSections; ++SectionHeader, --NumberOfSections)
        {
            CopyMemory(
                ModuleBase + SectionHeader->VirtualAddress,
                DllBase + SectionHeader->PointerToRawData,
                SectionHeader->SizeOfRawData
            );
        }

//        CopyMemory(ModuleBase, DllBase, (ULONG_PTR)SectionHeader - (ULONG_PTR)DllBase);
        CopyMemory(ModuleBase, DllBase, MEMORY_PAGE_SIZE);
    }

    if (FLAG_ON(MemDllInfo->Flags, LMD_IGNORE_IAT_DLL_MISSING))
    {
        WalkImportTableT(ModuleBase,
            WalkIATCallbackM(Data)
            {
                return STATUS_VALIDATE_CONTINUE;
            },
            nullptr
        );
    }

    Status = (ULONG_PTR)ModuleBase != NtHeader->OptionalHeader.ImageBase ? STATUS_IMAGE_NOT_AT_BASE : STATUS_SUCCESS;
    return Status;

CALL_ORIGINAL:
    return MemDllInfo->NtMapViewOfSection(SectionHandle, ProcessHandle, BaseAddress, ZeroBits, CommitSize, SectionOffset, ViewSize, InheritDisposition, AllocationType, Win32Protect);
}

NTSTATUS
STDCALL
LoadMemoryDll_NtQuerySection(
    HANDLE                      SectionHandle,
    SECTION_INFORMATION_CLASS   SectionInformationClass,
    PVOID                       SectionInformation,
    SIZE_T                      Length,
    PULONG                      ReturnLength
)
{
    PIMAGE_DOS_HEADER           DosHeader;
    PIMAGE_NT_HEADERS           NtHeaders;
    PIMAGE_OPTIONAL_HEADER      OptionalHeader;
    PLOAD_MEM_DLL_INFO          MemDllInfo;
    SECTION_IMAGE_INFORMATION  *ImageInfo;
    SECTION_BASIC_INFORMATION  *BasicInfo;

    MemDllInfo = GetLaodMemDllInfo();
    if (SectionHandle == nullptr || MemDllInfo->SectionHandle != SectionHandle)
        goto DEFAULT_PROC;

    DosHeader       = (PIMAGE_DOS_HEADER)MemDllInfo->MemDllBase;
    NtHeaders       = (PIMAGE_NT_HEADERS)((ULONG_PTR)DosHeader + DosHeader->e_lfanew);
    OptionalHeader  = &NtHeaders->OptionalHeader;

    switch (SectionInformationClass)
    {
        case SectionBasicInformation:
            BasicInfo                   = (SECTION_BASIC_INFORMATION *)SectionInformation;
            BasicInfo->BaseAddress      = MemDllInfo->MappedBase;
            BasicInfo->Attributes       = 0;
            BasicInfo->Size.QuadPart    = MemDllInfo->ViewSize;
            break;

        case SectionImageInformation:
            if (ReturnLength != nullptr)
                *ReturnLength = sizeof(*ImageInfo);

            if (Length < sizeof(*ImageInfo))
                return STATUS_BUFFER_TOO_SMALL;

            if (SectionInformation == nullptr)
                break;

            ImageInfo                        = (SECTION_IMAGE_INFORMATION *)SectionInformation;
            ImageInfo->TransferAddress       = (PVOID)((ULONG_PTR)DosHeader + OptionalHeader->AddressOfEntryPoint);
            ImageInfo->ZeroBits              = 0;
            ImageInfo->MaximumStackSize      = OptionalHeader->SizeOfStackReserve;
            ImageInfo->CommittedStackSize    = OptionalHeader->SizeOfStackCommit;
            ImageInfo->SubSystemType         = OptionalHeader->Subsystem;
            ImageInfo->SubSystemMinorVersion = OptionalHeader->MinorSubsystemVersion;
            ImageInfo->SubSystemMajorVersion = OptionalHeader->MajorSubsystemVersion;
            ImageInfo->GpValue               = 0;
            ImageInfo->ImageCharacteristics  = NtHeaders->FileHeader.Characteristics;
            ImageInfo->DllCharacteristics    = OptionalHeader->DllCharacteristics;
            ImageInfo->Machine               = NtHeaders->FileHeader.Machine;
            ImageInfo->ImageContainsCode     = 0; // OptionalHeader->SizeOfCode;
            ImageInfo->LoaderFlags           = OptionalHeader->LoaderFlags;
            ImageInfo->ImageFileSize         = (TYPE_OF(ImageInfo->ImageFileSize))MemDllInfo->DllBufferSize;
            ImageInfo->CheckSum              = (TYPE_OF(ImageInfo->CheckSum))OptionalHeader->CheckSum;
            break;

        case SectionRelocationInformation:
            if (SectionInformation != nullptr)
                *(PULONG_PTR)SectionInformation = (ULONG_PTR)MemDllInfo->MappedBase - (ULONG_PTR)OptionalHeader->ImageBase;

            if (ReturnLength != nullptr)
                *ReturnLength = sizeof(ULONG_PTR);

            break;

        default:
            goto DEFAULT_PROC;
    }

    return STATUS_SUCCESS;

DEFAULT_PROC:
    return MemDllInfo->NtQuerySection(SectionHandle, SectionInformationClass, SectionInformation, Length, ReturnLength);
}

NTSTATUS
STDCALL
LoadMemoryDll_NtClose(
    HANDLE Handle
)
{
    PLOAD_MEM_DLL_INFO MemDllInfo;

    MemDllInfo = GetLaodMemDllInfo();
    if (Handle != nullptr)
    {
        if (MemDllInfo->DllFileHandle == Handle)
        {
            MemDllInfo->DllFileHandle = nullptr;
        }
        else if (MemDllInfo->SectionHandle == Handle)
        {
            MemDllInfo->SectionHandle = nullptr;
        }
    }

    return MemDllInfo->NtClose(Handle);
}

NTSTATUS
LoadDllFromMemory(
    PVOID           DllBuffer,
    ULONG           DllBufferSize,
    PUNICODE_STRING ModuleFileName,
    PVOID*          ModuleHandle,
    ULONG           Flags /* = 0 */
)
{
    NTSTATUS            Status;
    ULONG               Length;
    PVOID               ModuleBase, ShadowNtdll;
    PLDR_MODULE         Ntdll;
    LOAD_MEM_DLL_INFO   MemDllInfo;
    PIMAGE_DOS_HEADER   DosHeader;
    PIMAGE_NT_HEADERS   NtHeader;
    WCHAR               Lz32DosPath[MAX_NTPATH];

    API_POINTER(NtQueryAttributesFile)  NtQueryAttributesFile;
    API_POINTER(NtOpenFile)             NtOpenFile;
    API_POINTER(NtCreateSection)        NtCreateSection;
    API_POINTER(NtMapViewOfSection)     NtMapViewOfSection;
    API_POINTER(NtClose)                NtClose;
    API_POINTER(NtQuerySection)         NtQuerySection;
    API_POINTER(LdrLoadDll)             LdrLoadDll;

    Ntdll = GetNtdllLdrModule();

    Status = LoadPeImage(Ntdll->FullDllName.Buffer, &ShadowNtdll, Ntdll->DllBase, LOAD_PE_IGNORE_IAT);
    if (NT_FAILED(Status))
    {
        return Status;
    }

    *(PVOID *)&NtQueryAttributesFile    = LookupExportTable(ShadowNtdll, NTDLL_NtQueryAttributesFile);
    *(PVOID *)&NtOpenFile               = LookupExportTable(ShadowNtdll, NTDLL_NtOpenFile);
    *(PVOID *)&NtCreateSection          = LookupExportTable(ShadowNtdll, NTDLL_NtCreateSection);
    *(PVOID *)&NtMapViewOfSection       = LookupExportTable(ShadowNtdll, NTDLL_NtMapViewOfSection);
    *(PVOID *)&NtClose                  = LookupExportTable(ShadowNtdll, NTDLL_NtClose);
    *(PVOID *)&NtQuerySection           = LookupExportTable(ShadowNtdll, NTDLL_NtQuerySection);
    *(PVOID *)&LdrLoadDll               = LookupExportTable(ShadowNtdll, NTDLL_LdrLoadDll);

    ZeroMemory(&MemDllInfo, sizeof(MemDllInfo));
    MemDllInfo.Context = LOAD_MEM_DLL_INFO_MAGIC;
    RtlPushFrame(&MemDllInfo);

    Status = STATUS_UNSUCCESSFUL;
    LOOP_ONCE
    {
        if (!RtlDosPathNameToNtPathName_U(ModuleFileName->Buffer, &MemDllInfo.MemDllFullPath, nullptr, nullptr))
            break;

        Length = Nt_GetSystemDirectory(Lz32DosPath, countof(Lz32DosPath));

        *(PULONG64)(Lz32DosPath + Length)       = TAG4W('lz32');
        *(PULONG64)(Lz32DosPath + Length + 4)   = TAG4W('.dll');
        Lz32DosPath[Length + 8]                 = 0;
        if (!RtlDosPathNameToNtPathName_U(Lz32DosPath, &MemDllInfo.Lz32Path, nullptr, nullptr))
            break;

        MemDllInfo.Flags            = Flags;
        MemDllInfo.MemDllBase       = DllBuffer;
        MemDllInfo.DllBufferSize    = DllBufferSize;
        DosHeader                   = (PIMAGE_DOS_HEADER)DllBuffer;
        NtHeader                    = (PIMAGE_NT_HEADERS)((ULONG_PTR)DllBuffer + DosHeader->e_lfanew);
        MemDllInfo.ViewSize         = NtHeader->OptionalHeader.SizeOfImage;

        Mp::PATCH_MEMORY_DATA p[] =
        {
            Mp::FunctionJumpVa(NtQueryAttributesFile, LoadMemoryDll_NtQueryAttributesFile, &MemDllInfo.NtQueryAttributesFile),
            Mp::FunctionJumpVa(NtOpenFile,            LoadMemoryDll_NtOpenFile,            &MemDllInfo.NtOpenFile),
            Mp::FunctionJumpVa(NtCreateSection,       LoadMemoryDll_NtCreateSection,       &MemDllInfo.NtCreateSection),
            Mp::FunctionJumpVa(NtMapViewOfSection,    LoadMemoryDll_NtMapViewOfSection,    &MemDllInfo.NtMapViewOfSection),
            Mp::FunctionJumpVa(NtClose,               LoadMemoryDll_NtClose,               &MemDllInfo.NtClose),
            Mp::FunctionJumpVa(NtQuerySection,        LoadMemoryDll_NtQuerySection,        &MemDllInfo.NtQuerySection),
        };

        Mp::PatchMemory(p, countof(p));
        Status = LdrLoadDll(nullptr, 0, ModuleFileName, &ModuleBase);
        Mp::RestoreMemory(p, countof(p));
        if (NT_FAILED(Status) && FLAG_OFF(Flags, LMD_MAPPED_DLL))
        {
//            UnmapDllSection(MemDllInfo.MemDllBase);
            break;
        }

        if (ModuleHandle != nullptr)
            *ModuleHandle = (HANDLE)ModuleBase;
    }

    RtlPopFrame(&MemDllInfo);
    RtlFreeUnicodeString(&MemDllInfo.MemDllFullPath);
    RtlFreeUnicodeString(&MemDllInfo.Lz32Path);

    UnloadPeImage(ShadowNtdll);

    return Status;
}

#endif // ML_USER_MODE

_ML_C_TAIL_

