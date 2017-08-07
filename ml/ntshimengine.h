#ifndef _NTSHIMENGINE_H_089a57ff_0a3f_4bd4_b4ea_37b0c469848f_
#define _NTSHIMENGINE_H_089a57ff_0a3f_4bd4_b4ea_37b0c469848f_

#include "ml/ntldr.h"

typedef
SHIM_HOOK_API_INFO*
(STDCALL
*GetHookAPIsRoutine)(
    IN  PCWSTR ShimCommandLine,
    IN  PCWSTR CompatibilityFixName,
    OUT PULONG HookApiCount
);

typedef
VOID
(STDCALL
*NotifyShimsRoutine)(
    IN SHIM_NOTIFY_REASON   Reason,
    IN PLDR_MODULE          LoadedDll
);

typedef struct
{
    WCHAR ShimEngineFullPath[MAX_PATH];
    ULONG SizeOfData;
    ULONG Magic;        // 0xAC0DEDAB

} SHIM_ENGINE_DATA, *PSHIM_ENGINE_DATA;

/*++

  if return success, load LayerDll

--*/

typedef
NTSTATUS
(STDCALL
*SE_InitializeEngine)(
    PUNICODE_STRING     LayerDll,
    PUNICODE_STRING     ExeFullPath,
    PSHIM_ENGINE_DATA   ShimData
);


/*++

  call before invoke dll EntryPoint
  return value seems be ignored
  must export

--*/

typedef
BOOLEAN
(STDCALL
*SE_InstallBeforeInitRoutine)(
    PUNICODE_STRING     ModuleLoading,
    PSHIM_ENGINE_DATA   ShimData
);


/*++

  call after invoke dll EntryPoint
  return FALSE will be unloaded
  must export

--*/

typedef
BOOLEAN
(STDCALL
*SE_InstallAfterInitRoutine)(
    PUNICODE_STRING     ModuleLoading,
    PSHIM_ENGINE_DATA   ShimData
);


typedef
VOID
(STDCALL
*SE_ShimDllLoaded)(
    PVOID DllHandle
);

/*++

  invoke before LdrpCallInitRoutine
  must export

--*/

typedef
VOID
(STDCALL
*SE_DllLoadedRoutine)(
    PLDR_DATA_TABLE_ENTRY DllLoaded
);

/*++

  invoke before NtUnmapViewOfSection and after decrement LoadCount
  must export

--*/

typedef
VOID
(STDCALL
*SE_DllUnloadedRoutine)(
    PLDR_DATA_TABLE_ENTRY DllUnLoading
);

/*++

  must export

--*/

typedef
VOID
(STDCALL
*SE_ProcessDyingRoutine)(
    VOID
);

typedef
VOID
(STDCALL
*SE_LdrEntryRemovedRoutine)(
    PLDR_DATA_TABLE_ENTRY EntryRemoving
);

/*++

  call when first LdrGetProcedureAddressEx invoked

--*/

typedef
VOID
(STDCALL
*SE_GetProcAddressLoadRoutine)(
    PLDR_DATA_TABLE_ENTRY LdrEntryLinks
);

NATIVE_API
BOOL
NTAPI
LdrInitShimEngineDynamic(
    PVOID ShimEngineModule
);

#endif // _NTSHIMENGINE_H_089a57ff_0a3f_4bd4_b4ea_37b0c469848f_