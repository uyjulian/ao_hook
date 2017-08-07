#include "mylibrary.h"


PIMAGE_RESOURCE_DATA_ENTRY
Nt_FindResource(
    PVOID   Module,
    PCWSTR  Name,
    PCWSTR  Type
)
{
    return ml::Native::Ldr::FindResource(Module, Name, Type);
}

PVOID
Nt_LoadResource(
    PVOID                       Module,
    PIMAGE_RESOURCE_DATA_ENTRY  ResourceDataEntry,
    PULONG                      Size
)
{
    return ml::Native::Ldr::LoadResource(Module, ResourceDataEntry, Size);
}
