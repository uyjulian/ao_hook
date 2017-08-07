#ifndef _NATIVETYPES_KERNEL_H_c07a308c_9b77_41fa_b475_9c2d5c0ae3e6
#define _NATIVETYPES_KERNEL_H_c07a308c_9b77_41fa_b475_9c2d5c0ae3e6


#define INFINITE ULONG_PTR_MAX

typedef USHORT ATOM;
typedef PVOID PRTL_CRITICAL_SECTION;

// typedef PROCESSINFOCLASS    PROCESS_INFORMATION_CLASS;
// typedef THREADINFOCLASS     THREAD_INFORMATION_CLASS;

typedef struct _KSERVICE_TABLE_DESCRIPTOR
{
    PULONG_PTR  Base;       // ssdt
    PULONG_PTR  Count;      // callout count of each system service
    ULONG_PTR   Limit;      // count of system call
    PUCHAR      Number;     // Size of arguments

} KSERVICE_TABLE_DESCRIPTOR, *PKSERVICE_TABLE_DESCRIPTOR;

///////////////////////////// apc /////////////////////////////////////

typedef enum _KAPC_ENVIRONMENT
{
    OriginalApcEnvironment,
    AttachedApcEnvironment,
    CurrentApcEnvironment,
    InsertApcEnvironment

} KAPC_ENVIRONMENT;

#endif // _NATIVETYPES_KERNEL_H_c07a308c_9b77_41fa_b475_9c2d5c0ae3e6