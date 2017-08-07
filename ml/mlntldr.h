#ifndef _MLNTLDR_H_ce875050_5f9f_4247_aca8_371c0807af3d_
#define _MLNTLDR_H_ce875050_5f9f_4247_aca8_371c0807af3d_

#include "ml/mlntprocess.h"

ML_NAMESPACE_BEGIN(Ldr);

inline PVOID GetExeModuleHandle()
{
    return (PVOID)Ps::CurrentPeb()->ImageBaseAddress;
}

PLDR_MODULE
FindLdrModuleByName(
    PUNICODE_STRING ModuleName
);

PLDR_MODULE
FindLdrModuleByHandle(
    PVOID BaseAddress
);

PIMAGE_RESOURCE_DATA_ENTRY
FindResource(
    PVOID   Module,
    PCWSTR  Name,
    PCWSTR  Type
);

PVOID
LoadResource(
    PVOID                       Module,
    PIMAGE_RESOURCE_DATA_ENTRY  ResourceDataEntry,
    PULONG                      Size = nullptr
);

#define LOAD_PE_IGNORE_IAT              0x00000001
#define LOAD_PE_IGNORE_RELOC            0x00000002
#define LOAD_PE_DLL_NOT_FOUND_CONTINUE  0x00000004
#define LOAD_PE_NOT_RESOLVE_PATH        0x00000010
#define LOAD_PE_TOP_TO_DOWN             0x00000020

NTSTATUS
LoadPeImage(
    PCWSTR      FullDllPath,
    PVOID*      DllBaseAddress,
    PVOID       OldBaseAddress   = nullptr,
    ULONG_PTR   Flags            = 0
);

NTSTATUS
UnloadPeImage(
    PVOID DllBase
);

NTSTATUS
RelocPeImage(
    PVOID   ImageBuffer,
    PVOID64 ImageDefaultBase,
    PVOID   ImageOldBase    = nullptr,
    PVOID   ImageNewBase    = nullptr
);

NTSTATUS
RelocBlock(
    PVOID                   ImageBase,
    LONG64                  ImageBaseOffset,
    PIMAGE_BASE_RELOCATION2 Relocation
);

PVOID
FASTCALL
GetRoutineAddress(
    PVOID ModuleBase,
    PCSTR ProcedureName
);

inline
PVOID
FASTCALL
GetRoutineAddress(
    PVOID ModuleBase,
    ULONG Ordinal
)
{
    return GetRoutineAddress(ModuleBase, (PCSTR)(ULONG_PTR)Ordinal);
}

PVOID
LoadDll(
    PCWSTR ModuleFileName
);

NTSTATUS
UnloadDll(
    PVOID DllHandle
);

ML_NAMESPACE_END_(Ldr);

#endif // _MLNTLDR_H_ce875050_5f9f_4247_aca8_371c0807af3d_