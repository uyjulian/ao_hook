#include "mylibrary.h"


ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Ldr)

/************************************************************************
  UserMode
************************************************************************/

#if ML_USER_MODE

PLDR_MODULE FindLdrModuleByName(PUNICODE_STRING ModuleName)
{
    PLDR_MODULE Ldr;
    PLIST_ENTRY LdrLink, NextLink;
    PVOID       LoaderLockCookie;
    NTSTATUS    Status;

    Status = LdrLockLoaderLock(0, NULL, &LoaderLockCookie);

    LdrLink = &Ps::CurrentPeb()->Ldr->InLoadOrderModuleList;
    NextLink = LdrLink->Flink;

    if (ModuleName == NULL)
    {
        if (NT_SUCCESS(Status))
            LdrUnlockLoaderLock(0, LoaderLockCookie);

        return FIELD_BASE(NextLink, LDR_MODULE, InLoadOrderLinks);
    }

    while (NextLink != LdrLink)
    {
        Ldr = FIELD_BASE(NextLink, LDR_MODULE, InLoadOrderLinks);

        if (RtlEqualUnicodeString(ModuleName, &Ldr->BaseDllName, TRUE))
        {
            if (NT_SUCCESS(Status))
                LdrUnlockLoaderLock(0, LoaderLockCookie);

            return Ldr;
        }

        NextLink = NextLink->Flink;
    }

    if (NT_SUCCESS(Status))
        LdrUnlockLoaderLock(0, LoaderLockCookie);

    return NULL;
}

PLDR_MODULE FindLdrModuleByHandle(PVOID BaseAddress)
{
    PLDR_MODULE Ldr;
    PLIST_ENTRY LdrLink, NextLink;

    if (BaseAddress != NULL)
    {
        NTSTATUS Status;

        Status = LdrFindEntryForAddress(BaseAddress, &Ldr);
        return NT_SUCCESS(Status) ? Ldr : NULL;
    }

    LdrLink = &Nt_CurrentPeb()->Ldr->InLoadOrderModuleList;
    NextLink = LdrLink->Flink;

    return FIELD_BASE(NextLink, LDR_MODULE, InLoadOrderLinks);
}

#endif // r3

ForceInline PVOID GetModuleBase(PVOID Module)
{
#if ML_USER_MODE
    Module = FindLdrModuleByHandle(Module)->DllBase;
#endif

    return Module;
}

PIMAGE_RESOURCE_DATA_ENTRY FindResource(PVOID Module, PCWSTR Name, PCWSTR Type)
{
    NTSTATUS                    Status;
    ULONG_PTR                   ResourceIdPath[3];
    PIMAGE_RESOURCE_DATA_ENTRY  ResourceDataEntry;

    ResourceIdPath[0] = (ULONG_PTR)Type;
    ResourceIdPath[1] = (ULONG_PTR)Name;
    ResourceIdPath[2] = 0;

    Status = LdrFindResource_U(GetModuleBase(Module), ResourceIdPath, countof(ResourceIdPath), &ResourceDataEntry);

    return NT_SUCCESS(Status) ? ResourceDataEntry : NULL;
}

PVOID LoadResource(PVOID Module, PIMAGE_RESOURCE_DATA_ENTRY ResourceDataEntry, PULONG Size)
{
    PVOID       Address;
    NTSTATUS    Status;

    Status = LdrAccessResource(GetModuleBase(Module), ResourceDataEntry, &Address, Size);

    return NT_SUCCESS(Status) ? Address : NULL;
}


/************************************************************************
  load pe image
************************************************************************/

#define LOAD_PE_MEMORY_TAG   TAG4('LPMT')

NTSTATUS
RelocBlock(
    PVOID                   ImageBase,
    LONG64                  ImageBaseOffset,
    PIMAGE_BASE_RELOCATION2 Relocation
)
{
    LONG        SizeOfBlock;
    PVOID       RelocateBase;
    PIMAGE_RELOCATION_ADDRESS_ENTRY TypeOffset;

    TypeOffset      = Relocation->TypeOffset;
    SizeOfBlock     = Relocation->SizeOfBlock;
    RelocateBase    = PtrAdd(ImageBase, Relocation->VirtualAddress);
    SizeOfBlock    -= sizeof(*Relocation) - sizeof(Relocation->TypeOffset);

    for (; SizeOfBlock > 0; ++TypeOffset, SizeOfBlock -= sizeof(*TypeOffset))
    {
        if (*(PUSHORT)TypeOffset == 0)
            continue;

        PLONG_PTR VA = (PLONG_PTR)PtrAdd(RelocateBase, TypeOffset->Offset);
        switch (TypeOffset->Type)
        {
            case IMAGE_REL_BASED_ABSOLUTE:
                break;

            case IMAGE_REL_BASED_HIGH:
                *(PUSHORT)VA += HIWORD((ULONG)ImageBaseOffset);
                break;

            case IMAGE_REL_BASED_LOW:
                *(PUSHORT)VA += LOWORD((ULONG)ImageBaseOffset);
                break;

            case IMAGE_REL_BASED_HIGHLOW:
                *(PLONG)VA += (ULONG)ImageBaseOffset;
                break;

            case IMAGE_REL_BASED_HIGHADJ:
                ++TypeOffset;
                SizeOfBlock -= sizeof(*TypeOffset);

                *(PUSHORT)VA += ((*(PUSHORT)VA << 16) + (ULONG)ImageBaseOffset + *(PSHORT)TypeOffset + 0x8000) >> 16;
                break;

            case IMAGE_REL_BASED_MIPS_JMPADDR:
            case IMAGE_REL_BASED_IA64_IMM64:
                break;

            case IMAGE_REL_BASED_DIR64:
                *(PLONG64)VA += (LONG64)ImageBaseOffset;
                break;

            default:
                break;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
RelocPeImage(
    PVOID   ImageBuffer,
    PVOID64 ImageDefaultBase,
    PVOID   ImageOldBase,
    PVOID   ImageNewBase
)
{
    LONG                            SizeOfBlock;
    PBYTE                           Base, RelocateBase;
    LONG64                          BaseOffset;
    ULONG                           SizeOfRelocation;
    PIMAGE_BASE_RELOCATION2         Relocation, RelocationEnd;
    PIMAGE_RELOCATION_ADDRESS_ENTRY TypeOffset;
    PIMAGE_DATA_DIRECTORY           DataDirectory;
    PIMAGE_NT_HEADERS32             NtHeaders32;
    PIMAGE_NT_HEADERS64             NtHeaders64;

    if (ImageNewBase == nullptr)
        ImageNewBase = ImageBuffer;

    NtHeaders32 = (PIMAGE_NT_HEADERS32)RtlImageNtHeader(ImageBuffer);

    switch (NtHeaders32->OptionalHeader.Magic)
    {
        case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
            DataDirectory = NtHeaders32->OptionalHeader.DataDirectory;
            break;

        case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
            NtHeaders64 = (PIMAGE_NT_HEADERS64)NtHeaders32;
            DataDirectory = NtHeaders64->OptionalHeader.DataDirectory;
            break;

        default:
            return STATUS_INVALID_IMAGE_FORMAT;
    }

    Relocation = (PIMAGE_BASE_RELOCATION2)PtrAdd(ImageBuffer, DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
    SizeOfRelocation = DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

    if ((PVOID)Relocation == ImageBuffer || SizeOfRelocation == 0)
        return STATUS_SUCCESS;

    RelocationEnd = PtrAdd(Relocation, SizeOfRelocation);

    Base        = (PBYTE)ImageBuffer;
    BaseOffset  = PtrSub(ImageOldBase == nullptr ? (LONG64)ImageNewBase : (LONG64)ImageOldBase, (LONG64)ImageDefaultBase);

    while (Relocation < RelocationEnd && Relocation->SizeOfBlock != 0)
    {
        RelocBlock(Base, BaseOffset, Relocation);
        Relocation = PtrAdd(Relocation, Relocation->SizeOfBlock);
    }

    return STATUS_SUCCESS;
}

#if ML_KERNEL_MODE

NTSTATUS ProcessStaticImport32(PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor, ULONG_PTR BaseAddress, ULONG_PTR Flags)
{
    return STATUS_NOT_IMPLEMENTED;
}

#else // user mode

NTSTATUS ProcessStaticImport32(PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor, ULONG_PTR BaseAddress, ULONG_PTR Flags)
{
    ULONG_PTR           Length;
    PVOID               DllBase;
    PCSTR               DllName;
    WCHAR               DllPathBuffer[MAX_NTPATH];
    UNICODE_STRING      DllPath;
    NTSTATUS            Status;
    PIMAGE_THUNK_DATA   OriginalThunk, FirstThunk;

    DllPath.Buffer = DllPathBuffer;
    DllPath.MaximumLength = sizeof(DllPathBuffer);
    for (; ImportDescriptor->Name != NULL && ImportDescriptor->FirstThunk != NULL; ++ImportDescriptor)
    {
        DllName = (PCSTR)BaseAddress + ImportDescriptor->Name;
        Nls::AnsiToUnicode(DllPathBuffer, countof(DllPathBuffer), DllName, -1, &Length);
        DllPath.Length = (USHORT)Length;
        Status = LdrLoadDll(NULL, 0, &DllPath, &DllBase);
        if (!NT_SUCCESS(Status))
        {
            if (FLAG_ON(Flags, LOAD_PE_DLL_NOT_FOUND_CONTINUE))
                continue;

            return Status;
        }

        OriginalThunk = (PIMAGE_THUNK_DATA)BaseAddress;
        if (ImportDescriptor->OriginalFirstThunk)
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->OriginalFirstThunk);
        else
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->FirstThunk);

        FirstThunk = (PIMAGE_THUNK_DATA)(BaseAddress + ImportDescriptor->FirstThunk);
        while (OriginalThunk->u1.AddressOfData != NULL)
        {
            LONG_PTR FunctionName;

            FunctionName = OriginalThunk->u1.AddressOfData;
            FunctionName = (FunctionName < 0) ? (USHORT)FunctionName : (FunctionName + BaseAddress + 2);
            *(PVOID *)&FirstThunk->u1.Function = GetRoutineAddress(DllBase, (PCSTR)FunctionName);
            ++OriginalThunk;
            ++FirstThunk;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS ProcessStaticImport64(PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor, ULONG_PTR BaseAddress, ULONG_PTR Flags)
{
    ULONG_PTR           Length;
    PVOID               DllBase;
    PCSTR               DllName;
    WCHAR               DllPathBuffer[MAX_NTPATH];
    UNICODE_STRING      DllPath;
    NTSTATUS            Status;
    PIMAGE_THUNK_DATA64 OriginalThunk, FirstThunk;

    DllPath.Buffer = DllPathBuffer;
    DllPath.MaximumLength = sizeof(DllPathBuffer);
    for (; ImportDescriptor->Name != NULL && ImportDescriptor->FirstThunk != NULL; ++ImportDescriptor)
    {
        DllName = (PCSTR)BaseAddress + ImportDescriptor->Name;
        Nls::AnsiToUnicode(DllPathBuffer, countof(DllPathBuffer), DllName, -1, &Length);
        DllPath.Length = (USHORT)Length;
        Status = LdrLoadDll(NULL, 0, &DllPath, &DllBase);
        if (!NT_SUCCESS(Status))
            return Status;

        OriginalThunk = (PIMAGE_THUNK_DATA64)BaseAddress;
        if (ImportDescriptor->OriginalFirstThunk)
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->OriginalFirstThunk);
        else
            OriginalThunk = PtrAdd(OriginalThunk, ImportDescriptor->FirstThunk);

        FirstThunk = (PIMAGE_THUNK_DATA64)(BaseAddress + ImportDescriptor->FirstThunk);
        while (OriginalThunk->u1.AddressOfData != NULL)
        {
            LONG_PTR FunctionName;

            FunctionName = (LONG_PTR)OriginalThunk->u1.AddressOfData;
            FunctionName = (FunctionName < 0) ? (USHORT)FunctionName : (FunctionName + BaseAddress + 2);
            *(PVOID *)&FirstThunk->u1.Function = GetRoutineAddress(DllBase, FunctionName);
            ++OriginalThunk;
            ++FirstThunk;
        }
    }

    return STATUS_SUCCESS;
}

#endif // ML_KERNEL_MODE

NTSTATUS
LoadPeImageWorker(
    PCWSTR      FullDllPath,
    PVOID*      DllBaseAddress,
    PVOID       OldBaseAddress,
    ULONG_PTR   Flags
)
{
    ULONG_PTR                   SizeOfHeaders, FileAlignment, SectionAlignment, SizeOfImage;
    PVOID64                     ImageBase;
    NTSTATUS                    Status;
    NtFileDisk                  file;
    IMAGE_DOS_HEADER            DosHeader;
    PIMAGE_OPTIONAL_HEADER      OptionalHeader32;
    PIMAGE_OPTIONAL_HEADER64    OptionalHeader64;
    PIMAGE_SECTION_HEADER       SectionHeader, Section;
    PIMAGE_IMPORT_DESCRIPTOR    ImportDescriptor;
    PIMAGE_DATA_DIRECTORY       DataDirectory;
    PBYTE                       Buffer;
    PVOID                       BaseAddress;

    union
    {
        IMAGE_NT_HEADERS            NtHeaders;
        IMAGE_NT_HEADERS64          NtHeaders64;
    };

    *DllBaseAddress = NULL;

    Status = file.Open(FullDllPath, FLAG_ON(Flags, LOAD_PE_NOT_RESOLVE_PATH) ? NFD_NOT_RESOLVE_PATH : 0);

    if (!NT_SUCCESS(Status))
        return Status;

    Status = file.Read(&DosHeader, sizeof(DosHeader));
    if (!NT_SUCCESS(Status))
        return Status;

    if (DosHeader.e_magic != IMAGE_DOS_SIGNATURE)
        return STATUS_INVALID_IMAGE_FORMAT;

    Status = file.Seek(DosHeader.e_lfanew, FILE_BEGIN);
    if (!NT_SUCCESS(Status))
        return Status;

    Status = file.Read(&NtHeaders, sizeof(NtHeaders) - sizeof(NtHeaders.OptionalHeader));
    if (!NT_SUCCESS(Status))
        return Status;

    if (NtHeaders.Signature != IMAGE_NT_SIGNATURE)
        return STATUS_INVALID_IMAGE_FORMAT;

    switch (NtHeaders.FileHeader.Machine)
    {
        case IMAGE_FILE_MACHINE_I386:
            OptionalHeader32 = (PIMAGE_OPTIONAL_HEADER)AllocStack(NtHeaders.FileHeader.SizeOfOptionalHeader);
            OptionalHeader64 = NULL;
            Status = file.Read(OptionalHeader32, NtHeaders.FileHeader.SizeOfOptionalHeader);
            break;

        case IMAGE_FILE_MACHINE_AMD64:
            OptionalHeader32 = NULL;
            OptionalHeader64 = (PIMAGE_OPTIONAL_HEADER64)AllocStack(NtHeaders64.FileHeader.SizeOfOptionalHeader);
            Status = file.Read(OptionalHeader64, NtHeaders64.FileHeader.SizeOfOptionalHeader);
            break;

        default:
            return STATUS_INVALID_IMAGE_FORMAT;
    }

    if (!NT_SUCCESS(Status))
        return Status;

    SectionHeader = (PIMAGE_SECTION_HEADER)AllocStack(NtHeaders.FileHeader.NumberOfSections * sizeof(*SectionHeader));
    Status = file.Read(SectionHeader, NtHeaders.FileHeader.NumberOfSections * sizeof(*SectionHeader));
    if (!NT_SUCCESS(Status))
        return Status;

    if (OptionalHeader32 != NULL)
    {
        SizeOfImage         = OptionalHeader32->SizeOfImage;
        SizeOfHeaders       = OptionalHeader32->SizeOfHeaders;
        FileAlignment       = OptionalHeader32->FileAlignment;
        SectionAlignment    = OptionalHeader32->SectionAlignment;
        ImageBase           = (PVOID64)OptionalHeader32->ImageBase;
        DataDirectory       = OptionalHeader32->DataDirectory;
    }
    else
    {
        SizeOfImage         = OptionalHeader64->SizeOfImage;
        SizeOfHeaders       = OptionalHeader64->SizeOfHeaders;
        FileAlignment       = OptionalHeader64->FileAlignment;
        SectionAlignment    = OptionalHeader64->SectionAlignment;
        ImageBase           = (PVOID64)OptionalHeader64->ImageBase;
        DataDirectory       = OptionalHeader64->DataDirectory;
    }

#if ML_KERNEL_MODE

    PMDL                Mdl;
    PHYSICAL_ADDRESS    LowAddress, HighAddress, SkipBytes;

    LowAddress.QuadPart     = 0;
    HighAddress.QuadPart    = -1;
    SkipBytes.QuadPart      = 0;

    BaseAddress = MmAllocateMappingAddress(SizeOfImage, LOAD_PE_MEMORY_TAG);
    if (BaseAddress == NULL)
        return STATUS_NO_MEMORY;

    Mdl = MmAllocatePagesForMdl(LowAddress, HighAddress, SkipBytes, SizeOfImage);
    if (Mdl == NULL)
    {
        MmFreeMappingAddress(BaseAddress, LOAD_PE_MEMORY_TAG);
        return STATUS_NO_MEMORY;
    }

    BaseAddress = MmMapLockedPagesWithReservedMapping(BaseAddress, LOAD_PE_MEMORY_TAG, Mdl, MmCached);
    if (BaseAddress == NULL)
    {
        MmFreePagesFromMdl(Mdl);
        ExFreePool(Mdl);
        MmFreeMappingAddress(BaseAddress, LOAD_PE_MEMORY_TAG);

        return STATUS_NO_MEMORY;
    }

    *(PMDL *)&DosHeader.e_res2 = Mdl;

    *DllBaseAddress = BaseAddress;

    Status = MmProtectMdlSystemAddress(Mdl, PAGE_EXECUTE_READWRITE);
    FAIL_RETURN(Status);

#else // r3

    Status = Mm::AllocVirtualMemory(DllBaseAddress, SizeOfImage, PAGE_EXECUTE_READWRITE, MEM_RESERVE | MEM_COMMIT | (FLAG_ON(Flags, LOAD_PE_TOP_TO_DOWN) ? MEM_TOP_DOWN : 0));
    if (!NT_SUCCESS(Status))
        return Status;

    BaseAddress = *DllBaseAddress;

#endif

    FAIL_RETURN(file.Seek(0ll, FILE_BEGIN));
    FAIL_RETURN(file.Read(BaseAddress, SizeOfHeaders));

    Section = SectionHeader;
    for (ULONG_PTR NumberOfSection = NtHeaders.FileHeader.NumberOfSections; NumberOfSection; ++Section, --NumberOfSection)
    {
        ULONG_PTR PointerToRawData, SizeOfRawData, VirtualAddress;

        SizeOfRawData = Section->SizeOfRawData;
        SizeOfRawData = ROUND_UP(SizeOfRawData, FileAlignment);
        if (SizeOfRawData == 0)
            continue;

        PointerToRawData = Section->PointerToRawData;
        PointerToRawData = ROUND_DOWN(PointerToRawData, FileAlignment);
        Status = file.Seek(PointerToRawData, FILE_BEGIN);
        if (!NT_SUCCESS(Status))
            return Status;

        VirtualAddress = Section->VirtualAddress;
        VirtualAddress = ROUND_DOWN(VirtualAddress, SectionAlignment);
        Buffer = (PBYTE)BaseAddress + VirtualAddress;
        Status = file.Read(Buffer, SizeOfRawData);
        if (!NT_SUCCESS(Status))
            return Status;
    }

    Buffer = (PBYTE)BaseAddress;

    if (!FLAG_ON(Flags, LOAD_PE_IGNORE_RELOC)) LOOP_ONCE
    {
        if (!ValidateDataDirectory(&DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC], SizeOfImage))
            break;

        Status = RelocPeImage(Buffer, ImageBase, OldBaseAddress);
    }

#if !ML_KERNEL_MODE

    if (!FLAG_ON(Flags, LOAD_PE_IGNORE_IAT) &&
        ValidateDataDirectory(&DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT], SizeOfImage))
    {
        ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress + Buffer);

        switch (NtHeaders.FileHeader.Machine)
        {
            case IMAGE_FILE_MACHINE_I386:
                Status = ProcessStaticImport32(ImportDescriptor, (ULONG_PTR)Buffer, Flags);
                break;

            case IMAGE_FILE_MACHINE_AMD64:
                Status = ProcessStaticImport64(ImportDescriptor, (ULONG_PTR)Buffer, Flags);
                break;
        }
    }

#else

    Status = STATUS_SUCCESS;

#endif // r3

    return Status;
}

NTSTATUS UnloadPeImage(PVOID DllBase)
{
    if (DllBase == NULL)
        return STATUS_INVALID_ADDRESS;

#if ML_KERNEL_MODE

    PIMAGE_DOS_HEADER   DosHeader;
    PMDL                Mdl;

    DosHeader   = (PIMAGE_DOS_HEADER)DllBase;
    Mdl         = *(PMDL *)&DosHeader->e_res2;

    if (Mdl == NULL)
        return STATUS_UNSUCCESSFUL;

    MmUnmapReservedMapping(DllBase, LOAD_PE_MEMORY_TAG, Mdl);
    MmFreePagesFromMdl(Mdl);
    ExFreePool(Mdl);
    MmFreeMappingAddress(DllBase, LOAD_PE_MEMORY_TAG);

    return STATUS_SUCCESS;

#else // r3

    return Mm::FreeVirtualMemory(DllBase);

#endif // ML_KERNEL_MODE
}

NTSTATUS
LoadPeImage(
    PCWSTR      FullDllPath,
    PVOID*      DllBaseAddress,
    PVOID       OldBaseAddress,
    ULONG_PTR   Flags
)
{
    NTSTATUS Status;

    Status = LoadPeImageWorker(FullDllPath, DllBaseAddress, OldBaseAddress, Flags);
    if (!NT_SUCCESS(Status) && *DllBaseAddress != NULL)
        UnloadPeImage(*DllBaseAddress);

    return Status;
}

PVOID FASTCALL GetRoutineAddress(PVOID ModuleBase, LPCSTR RoutineName)
{

#if ML_USER_MODE

    PVOID       ProcAddress;
    LONG        Ordinal;
    NTSTATUS    Status;
    ANSI_STRING ProcString, *ProcName;

    if ((ULONG_PTR)RoutineName > 0xFFFF)
    {
        RtlInitAnsiString(&ProcString, RoutineName);
        ProcName    = &ProcString;
        Ordinal     = 0;
    }
    else
    {
        ProcName    = NULL;
        Ordinal     = (LONG)(LONG_PTR)RoutineName;
    }

    Status = LdrGetProcedureAddress(ModuleBase, ProcName, (USHORT)Ordinal, &ProcAddress);
    if (!NT_SUCCESS(Status))
        return NULL;

    return ProcAddress == (PVOID)ModuleBase ? NULL : (PVOID)ProcAddress;

#else

    return 0;

#endif

}

#if ML_USER_MODE

PVOID
LoadDll(
    PCWSTR ModuleFileName
)
{
    PVOID           ModuleBase;
    NTSTATUS        Status;
    UNICODE_STRING  ModuleFile;

    RtlInitUnicodeString(&ModuleFile, ModuleFileName);

    Status = LdrLoadDll(NULL, NULL, &ModuleFile, &ModuleBase);

    return NT_SUCCESS(Status) ? ModuleBase : NULL;
}

NTSTATUS
UnloadDll(
    PVOID DllHandle
)
{
    return LdrUnloadDll(DllHandle);
}

#endif // r3

ML_NAMESPACE_END_(Ldr);

ML_NAMESPACE_END_(Native)



ML_NAMESPACE_END

