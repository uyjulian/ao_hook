#ifndef _MYLIBRARY_H_9fb91137_38ac_4d5e_8572_b6e12d23cb15_
#define _MYLIBRARY_H_9fb91137_38ac_4d5e_8572_b6e12d23cb15_

#if !defined(USE_NT_VER)
    #define USE_NT_VER 1
#endif

#define ML_DISABLE_THIRD_LIB_UCL 1

#if ML_KERNEL_MODE

// #include "Include\MyLibraryKernel.h"

#else
#include "ml/mylibraryuser.h"
#endif

#if CPP_DEFINED
#endif

ML_NAMESPACE

LONG_PTR MlInitialize();
LONG_PTR MlUnInitialize();

ML_NAMESPACE_END;

#if CPP_DEFINED
#include "ml/mlns.h"
#if ML_KERNEL_MODE

ML_NAMESPACE_BEGIN(WindowsVersionsInfoClass)

enum
{
    Windows7            = 0x00000601,
    Windows7SP1         = 0x01000601,
    Windows2008R2       = 0x80000601,

    Windows8            = 0x00000602,
    WindowsServer2012   = 0x80000602,

    Windows81           = 0x00000603,
    WindowsServer2012R2 = 0x80000603,

    InvalidVersion      = 0xFFFFFFFF,
};

inline ULONG GetWindowsVersion()
{
    NTSTATUS                        Status;
    ULONG                           CSDVersion;
    RTL_OSVERSIONINFOEXW            Version;
    PKEY_VALUE_PARTIAL_INFORMATION  Value;

    static ULONG WindowsVersion = 0;

    if (WindowsVersion != 0)
    {
        return WindowsVersion;
    }

    Version.dwOSVersionInfoSize = sizeof(Version);
    Status = RtlGetVersion((PRTL_OSVERSIONINFOW)&Version);
    if (NT_FAILED(Status))
        return InvalidVersion;

    Status = Reg::GetKeyValue(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\Windows", L"CSDVersion", &Value);
    if (NT_FAILED(Status))
        return InvalidVersion;

    if (Value->Type != REG_DWORD || Value->DataLength != sizeof(CSDVersion))
    {
        FreeKeyInfo(Value);
        return InvalidVersion;
    }

    CSDVersion = *(PULONG)Value->Data;
    FreeKeyInfo(Value);

    WindowsVersion = (Version.dwMajorVersion << 8 | Version.dwMinorVersion | ((CSDVersion << 16) & 0x7FFF0000)) | (Version.wProductType != VER_NT_WORKSTATION ? 0x80000000 : 0);

    return WindowsVersion;
}

inline BOOL IsSupportedVersion()
{
    switch (WindowsVersionsInfoClass::GetWindowsVersion())
    {
        case WindowsVersionsInfoClass::Windows7:
        case WindowsVersionsInfoClass::Windows7SP1:
        case WindowsVersionsInfoClass::Windows8:
        case WindowsVersionsInfoClass::Windows81:
            return TRUE;

        default:
            return FALSE;
    }
}

ML_NAMESPACE_END_(WindowsVersionsInfoClass);

#pragma push_macro("DEFINE_PROPERTY")
#pragma push_macro("DECLARE_ACCESSOR")
#pragma push_macro("FIELD_ACCESSOR")
#pragma push_macro("SET_ACCESSOR")
#pragma push_macro("CALL_ACCESSOR")

#undef DEFINE_PROPERTY
#undef DECLARE_ACCESSOR
#undef FIELD_ACCESSOR
#undef SET_ACCESSOR
#undef CALL_ACCESSOR


typedef struct _OBJECT_TYPE_INITIALIZER_WIN7_760X               // 25 elements, 0x50 bytes (sizeof)
{
/*0x000*/     USHORT       Length;
              union                                             // 2 elements, 0x1 bytes (sizeof)
              {
/*0x002*/         UCHAR        ObjectTypeFlags;
                  struct                                        // 7 elements, 0x1 bytes (sizeof)
                  {
/*0x002*/             UCHAR        CaseInsensitive : 1;         // 0 BitPosition
/*0x002*/             UCHAR        UnnamedObjectsOnly : 1;      // 1 BitPosition
/*0x002*/             UCHAR        UseDefaultObject : 1;        // 2 BitPosition
/*0x002*/             UCHAR        SecurityRequired : 1;        // 3 BitPosition
/*0x002*/             UCHAR        MaintainHandleCount : 1;     // 4 BitPosition
/*0x002*/             UCHAR        MaintainTypeList : 1;        // 5 BitPosition
/*0x002*/             UCHAR        SupportsObjectCallbacks : 1; // 6 BitPosition
                  };
              };
/*0x004*/     ULONG      ObjectTypeCode;
/*0x008*/     ULONG      InvalidAttributes;
/*0x00C*/     struct _GENERIC_MAPPING GenericMapping;           // 4 elements, 0x10 bytes (sizeof)
/*0x01C*/     ULONG      ValidAccessMask;
/*0x020*/     ULONG      RetainAccess;
/*0x024*/     enum _POOL_TYPE PoolType;
/*0x028*/     ULONG      DefaultPagedPoolCharge;
/*0x02C*/     ULONG      DefaultNonPagedPoolCharge;
/*0x030*/     PVOID      DumpProcedure;                       // FUNCT_00A4_0ED5_DumpProcedure*
/*0x034*/     PVOID      OpenProcedure;                       // FUNCT_000F_0EDD_OpenProcedure*
/*0x038*/     PVOID      CloseProcedure;                      // FUNCT_00A4_0EEB_CloseProcedure*
/*0x03C*/     PVOID      DeleteProcedure;                     // FUNCT_00A4_0662_Free_InterfaceReference_InterfaceDereference_DeleteProcedure_WorkerRoutine_Callback_ReleaseFromLazyWrite_ReleaseFromReadAhead*
/*0x040*/     PVOID      ParseProcedure;                      // FUNCT_000F_0EF1_ParseProcedure*
/*0x044*/     PVOID      SecurityProcedure;                   // FUNCT_000F_0EFD_SecurityProcedure*
/*0x048*/     PVOID      QueryNameProcedure;                  // FUNCT_000F_0F0E_QueryNameProcedure*
/*0x04C*/     PVOID      OkayToCloseProcedure;                // FUNCT_0067_0F16_OkayToCloseProcedure*

} OBJECT_TYPE_INITIALIZER_WIN7_760X, *POBJECT_TYPE_INITIALIZER_WIN7_760X;


typedef struct _OBJECT_TYPE_INITIALIZER_WIN8_9X00                    // 29 elements, 0x58 bytes (sizeof)
{
/*0x000*/     USHORT       Length;
              union                                             // 2 elements, 0x1 bytes (sizeof)
              {
/*0x002*/         UCHAR        ObjectTypeFlags;
                  struct                                        // 8 elements, 0x1 bytes (sizeof)
                  {
/*0x002*/             UCHAR        CaseInsensitive : 1;         // 0 BitPosition
/*0x002*/             UCHAR        UnnamedObjectsOnly : 1;      // 1 BitPosition
/*0x002*/             UCHAR        UseDefaultObject : 1;        // 2 BitPosition
/*0x002*/             UCHAR        SecurityRequired : 1;        // 3 BitPosition
/*0x002*/             UCHAR        MaintainHandleCount : 1;     // 4 BitPosition
/*0x002*/             UCHAR        MaintainTypeList : 1;        // 5 BitPosition
/*0x002*/             UCHAR        SupportsObjectCallbacks : 1; // 6 BitPosition
/*0x002*/             UCHAR        CacheAligned : 1;            // 7 BitPosition
                  };
              };

/*0x004*/     ULONG      ObjectTypeCode;
/*0x008*/     ULONG      InvalidAttributes;
/*0x00C*/     struct _GENERIC_MAPPING GenericMapping;           // 4 elements, 0x10 bytes (sizeof)
/*0x01C*/     ULONG      ValidAccessMask;
/*0x020*/     ULONG      RetainAccess;
/*0x024*/     enum _POOL_TYPE PoolType;
/*0x028*/     ULONG      DefaultPagedPoolCharge;
/*0x02C*/     ULONG      DefaultNonPagedPoolCharge;
/*0x030*/     PVOID      DumpProcedure;                       // FUNCT_00BC_140F_DumpProcedure*
/*0x034*/     PVOID      OpenProcedure;                       // FUNCT_0072_1417_OpenProcedure*
/*0x038*/     PVOID      CloseProcedure;                      // FUNCT_00BC_1425_CloseProcedure*
/*0x03C*/     PVOID      DeleteProcedure;                     // FUNCT_00BC_06F1_Free_WorkerRoutine_DeleteCallback_InterfaceReference_InterfaceDereference_DeleteProcedure_DevicePowerRequired_DevicePowerNotRequired_Callback_ReleaseFromLazyWrite_ReleaseFromReadAhead*
/*0x040*/     PVOID      ParseProcedure;                      // FUNCT_0072_142B_ParseProcedure*
/*0x044*/     PVOID      SecurityProcedure;                   // FUNCT_0072_1437_SecurityProcedure*
/*0x048*/     PVOID      QueryNameProcedure;                  // FUNCT_0072_1448_QueryNameProcedure*
/*0x04C*/     PVOID      OkayToCloseProcedure;                // FUNCT_007B_1450_OkayToCloseProcedure*
/*0x050*/     ULONG      WaitObjectFlagMask;
/*0x054*/     USHORT     WaitObjectFlagOffset;
/*0x056*/     USHORT     WaitObjectPointerOffset;

} OBJECT_TYPE_INITIALIZER_WIN8_9X00, *POBJECT_TYPE_INITIALIZER_WIN8_9X00;


typedef struct _OBJECT_TYPE_WIN8                            // 12 elements, 0x90 bytes (sizeof)
{
/*0x000*/     LIST_ENTRY        TypeList;                   // 2 elements, 0x8 bytes (sizeof)
/*0x008*/     UNICODE_STRING    Name;                       // 3 elements, 0x8 bytes (sizeof)
/*0x010*/     PVOID             DefaultObject;              // VOID*
/*0x014*/     UCHAR             Index;
/*0x018*/     ULONG             TotalNumberOfObjects;
/*0x01C*/     ULONG             TotalNumberOfHandles;
/*0x020*/     ULONG             HighWaterNumberOfObjects;
/*0x024*/     ULONG             HighWaterNumberOfHandles;
              OBJECT_TYPE_INITIALIZER_WIN8_9X00 TypeInfo[1];
#if 0
/*0x028*/     struct _OBJECT_TYPE_INITIALIZER TypeInfo;     // 29 elements, 0x58 bytes (sizeof)
/*0x080*/     struct _EX_PUSH_LOCK TypeLock;                // 7 elements, 0x4 bytes (sizeof)
/*0x084*/     ULONG32      Key;
/*0x088*/     struct _LIST_ENTRY CallbackList;              // 2 elements, 0x8 bytes (sizeof)
#endif

} OBJECT_TYPE_WIN8_9X00, *POBJECT_TYPE_WIN8_9X00;

typedef OBJECT_TYPE_WIN8_9X00 OBJECT_TYPE_WIN7_760X, *POBJECT_TYPE_WIN7_760X;


template<class _FIELD_TYPE, ULONG_PTR FieldOffset>
ForceInline _FIELD_TYPE& FieldAccessor(PVOID Object)
{
    return *(_FIELD_TYPE *)PtrAdd(Object, FieldOffset);
}


#define DEFINE_PROPERTY_(_RetType, _Object, _Type, _Field) \
            READONLY_PROPERTY(_RetType, _Field) \
            { \
                CALL_ACCESSOR(_Object, _Field); \
            }

#define DEFINE_PROPERTY(_Object, _Type, _Field) DEFINE_PROPERTY_(FIELD_TYPE(_Type, _Field)&, _Object, _Type, _Field)


#define DECLARE_ACCESSOR_(_RetType, _Object, _Type, _Field) \
            protected: _RetType (*Accessor_##_Field)(PVOID Object);\
            public: DEFINE_PROPERTY_(_RetType, _Object, _Type, _Field)

#define DECLARE_ACCESSOR(_Object, _Type, _Field) DECLARE_ACCESSOR_(FIELD_TYPE(_Type, _Field)&, _Object, _Type, _Field)


#define FIELD_ACCESSOR(_Type, _Field) FieldAccessor<FIELD_TYPE(_Type, _Field), FIELD_OFFSET(_Type,_Field)>

#define SET_ACCESSOR(_Type, _Field) \
            IF_EXIST(_Type::_Field) \
            { \
                *(PVOID *)&Accessor_##_Field = PtrAdd((PVOID)0, FIELD_ACCESSOR(_Type, _Field)) \
            } \
            IF_NOT_EXIST(_Type::_Field) \
            { \
                *(PVOID *)&Accessor_##_Field = PtrSub(IMAGE_INVALID_VA, 0x5432) \
            }

#define CALL_ACCESSOR(_Object, _Field) return this->Accessor_##_Field(_Object)

class ML_OBJECT_TYPE
{
protected:
    POBJECT_TYPE_WIN8_9X00 ObjectType;

public:

    BOOL Success;

    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, TypeList);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, Name);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, DefaultObject);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, Index);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, TotalNumberOfObjects);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, TotalNumberOfHandles);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, HighWaterNumberOfObjects);
    DECLARE_ACCESSOR(this->ObjectType, OBJECT_TYPE_WIN8_9X00, HighWaterNumberOfHandles);
    DECLARE_ACCESSOR_(POBJECT_TYPE_INITIALIZER, this->ObjectType, OBJECT_TYPE_WIN8_9X00, TypeInfo);

    NoInline ML_OBJECT_TYPE(POBJECT_TYPE ObjectType = nullptr)
    {
        this->Success = FALSE;

        if (WindowsVersionsInfoClass::IsSupportedVersion() == FALSE)
            return;

        *(POBJECT_TYPE *)&this->ObjectType = ObjectType;
        Initialize<OBJECT_TYPE_WIN8_9X00>();

        this->Success = TRUE;
    }

    ML_OBJECT_TYPE* operator->()
    {
        return this;
    }

    operator POBJECT_TYPE()
    {
        return (POBJECT_TYPE)this->ObjectType;
    }

protected:

    template<class OBJECT_TYPE_TYPE>
    VOID Initialize()
    {
        SET_ACCESSOR(OBJECT_TYPE_TYPE, TypeList);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, Name);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, DefaultObject);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, Index);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, TotalNumberOfObjects);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, TotalNumberOfHandles);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, HighWaterNumberOfObjects);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, HighWaterNumberOfHandles);
        SET_ACCESSOR(OBJECT_TYPE_TYPE, TypeInfo);
    }
};

class ML_OBJECT_TYPE_INITIALIZER_DATA
{
protected:
    POBJECT_TYPE_INITIALIZER ObjectTypeInitializerPointer;

    union
    {
        OBJECT_TYPE_INITIALIZER_WIN7_760X   Win7;
        OBJECT_TYPE_INITIALIZER_WIN8_9X00   Win8;

    } ObjectTypeInitializer;

public:

    BOOL Success;

    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, Length);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, ObjectTypeFlags);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, ObjectTypeCode);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, InvalidAttributes);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, GenericMapping);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, ValidAccessMask);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, RetainAccess);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, PoolType);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, DefaultPagedPoolCharge);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, DefaultNonPagedPoolCharge);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, DumpProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, OpenProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, CloseProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, DeleteProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, ParseProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, SecurityProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, QueryNameProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, OkayToCloseProcedure);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, WaitObjectFlagMask);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, WaitObjectFlagOffset);
    DECLARE_ACCESSOR(&this->ObjectTypeInitializer, OBJECT_TYPE_INITIALIZER_WIN8_9X00, WaitObjectPointerOffset);

    NoInline ML_OBJECT_TYPE_INITIALIZER_DATA(POBJECT_TYPE ObjectType = nullptr)
    {
        *(PVOID *)&this->ObjectTypeInitializerPointer = IMAGE_INVALID_VA;
        this->Success = FALSE;

        switch (WindowsVersionsInfoClass::GetWindowsVersion())
        {
            case WindowsVersionsInfoClass::Windows7:
            case WindowsVersionsInfoClass::Windows7SP1:
                InitializeObjectTypeInitializer(&this->ObjectTypeInitializer.Win7);
                break;

            case WindowsVersionsInfoClass::Windows8:
            case WindowsVersionsInfoClass::Windows81:
                InitializeObjectTypeInitializer(&this->ObjectTypeInitializer.Win8);
                break;

            default:
                return;
        }

        if (ObjectType != nullptr)
            (*this) = ObjectType;

        this->Success = TRUE;
    }

    ML_OBJECT_TYPE_INITIALIZER_DATA* operator->()
    {
        return this;
    }

    NoInline ML_OBJECT_TYPE_INITIALIZER_DATA& operator=(POBJECT_TYPE ObjectType)
    {
        ML_OBJECT_TYPE Type = ObjectType;

        CopyMemory(&this->ObjectTypeInitializer, Type->TypeInfo, this->Length);
        return *this;
    }

    operator POBJECT_TYPE_INITIALIZER()
    {
        return this->ObjectTypeInitializerPointer;
    }

protected:

    POBJECT_TYPE_INITIALIZER FindObjectTypeInitializerFromObjectType(POBJECT_TYPE ObjectType)
    {
        USHORT  Length;
        PBYTE   Buffer;

        Length = this->Length;
        Buffer = (PBYTE)ObjectType;

        for (;; Buffer += sizeof(ULONG_PTR))
        {
            if (*(PUSHORT)Buffer != Length)
                continue;
        }
    }

    template<class OBJECT_TYPE_INITIALIZER_TYPE>
    VOID InitializeObjectTypeInitializer(OBJECT_TYPE_INITIALIZER_TYPE *Ptr)
    {
        *(OBJECT_TYPE_INITIALIZER_TYPE **)&this->ObjectTypeInitializerPointer = Ptr;

        ZeroMemory(Ptr, sizeof(OBJECT_TYPE_INITIALIZER_TYPE));
        Ptr->Length = sizeof(*Ptr);

        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, Length);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, ObjectTypeFlags);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, ObjectTypeCode);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, InvalidAttributes);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, GenericMapping);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, ValidAccessMask);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, RetainAccess);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, PoolType);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, DefaultPagedPoolCharge);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, DefaultNonPagedPoolCharge);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, DumpProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, OpenProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, CloseProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, DeleteProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, ParseProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, SecurityProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, QueryNameProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, OkayToCloseProcedure);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, WaitObjectFlagMask);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, WaitObjectFlagOffset);
        SET_ACCESSOR(OBJECT_TYPE_INITIALIZER_TYPE, WaitObjectPointerOffset);
    }
};


class ML_EPROCESS_DYNAMIC
{
protected:
    PEPROCESS Process;

    static ULONG_PTR Offset_ProcessLock;
    static ULONG_PTR Offset_RundownProtect;
    static ULONG_PTR Offset_UniqueProcessId;

public:
    ML_EPROCESS_DYNAMIC(PEPROCESS Process = nullptr)
    {
        (*this) = Process;
    }

    ML_EPROCESS_DYNAMIC& operator=(PEPROCESS Process)
    {
        this->Process = Process;
        return *this;
    }

    operator PEPROCESS()
    {
        return this->Process;
    }

    READONLY_PROPERTY(EX_RUNDOWN_REF&, RundownProtect)
    {
        return *(PEX_RUNDOWN_REF)PtrAdd(this->Process, Offset_RundownProtect);
    }

    READONLY_PROPERTY(HANDLE, UniqueProcessId)
    {
        return *(PHANDLE)PtrAdd(this->Process, Offset_UniqueProcessId);
    }

    ML_EPROCESS_DYNAMIC* operator->()
    {
        return this;
    }

    NoInline static NTSTATUS InitializeOffset()
    {
        ULONG_PTR   Index, OffsetTable[0x1000 / sizeof(ULONG_PTR)];
        PULONG_PTR  Buffer;

        Index = 0;
        FOR_EACH_ARRAY(Buffer, OffsetTable)
        {
            *Buffer = Index++;
        }

        Index = (ULONG_PTR)PsGetProcessId((PEPROCESS)OffsetTable);

        Offset_UniqueProcessId = Index * sizeof(Index);
        Offset_RundownProtect = PtrSub(Offset_UniqueProcessId, sizeof(((ML_EPROCESS_DYNAMIC *)0)->RundownProtect));

        return STATUS_SUCCESS;
    }
};

DECL_SELECTANY ULONG_PTR ML_EPROCESS_DYNAMIC::Offset_ProcessLock = 0;
DECL_SELECTANY ULONG_PTR ML_EPROCESS_DYNAMIC::Offset_RundownProtect = 0;
DECL_SELECTANY ULONG_PTR ML_EPROCESS_DYNAMIC::Offset_UniqueProcessId = 0;



class ML_ETHREAD_DYNAMIC
{
protected:
    PETHREAD Thread;

    static ULONG_PTR Offset_Teb;
    static ULONG_PTR Offset_CreateTime;

public:
    ML_ETHREAD_DYNAMIC(PETHREAD Thread = nullptr)
    {
        (*this) = Thread;
    }

    ML_ETHREAD_DYNAMIC& operator=(PETHREAD Thread)
    {
        this->Thread = Thread;
        return *this;
    }

    operator PETHREAD()
    {
        return this->Thread;
    }

    PROPERTY(PVOID, Teb);

    GET(Teb)
    {
        return *(PVOID *)PtrAdd(this->Thread, Offset_Teb);
    }

    SET(Teb)
    {
        *(PVOID *)PtrAdd(this->Thread, Offset_Teb) = value;
    }

    PROPERTY(LARGE_INTEGER&, CreateTime);

    GET(CreateTime)
    {
        return *(PLARGE_INTEGER)PtrAdd(this->Thread, Offset_CreateTime);
    }

    SET(CreateTime)
    {
        ((PLARGE_INTEGER)PtrAdd(this->Thread, Offset_CreateTime))->QuadPart = value.QuadPart;
   }

    ML_ETHREAD_DYNAMIC* operator->()
    {
        return this;
    }

    NoInline static NTSTATUS InitializeOffset()
    {
        NTSTATUS            Status;
        ULONG_PTR           Index, OffsetTable[0x1000 / sizeof(ULONG_PTR)];
        PULONG_PTR          Buffer;
        KERNEL_USER_TIMES   Times;
        HANDLE              Handle;
        PLARGE_INTEGER      Begin, End;

        Index = 0;
        FOR_EACH_ARRAY(Buffer, OffsetTable)
        {
            *Buffer = Index++;
        }

        Index = (ULONG_PTR)PsGetThreadTeb((PETHREAD)OffsetTable);
        Offset_Teb = Index * sizeof(Index);

        Status = ThreadIdToHandleEx(&Handle, (ULONG_PTR)PsGetCurrentThreadId());
        FAIL_RETURN(Status);

        Status = ZwQueryInformationThread(Handle, ThreadTimes, &Times, sizeof(Times), nullptr);
        ZwClose(Handle);
        FAIL_RETURN(Status);

        Begin = (PLARGE_INTEGER)PsGetCurrentThread();
        End = PtrAdd(Begin, 0x1000);
        for (; Begin < End; ++Begin)
        {
            if (Begin->QuadPart == Times.CreateTime.QuadPart)
            {
                Offset_CreateTime = PtrOffset(Begin, PsGetCurrentThread());
                return STATUS_SUCCESS;
            }
        }

        return STATUS_NOT_FOUND;
    }
};

DECL_SELECTANY ULONG_PTR ML_ETHREAD_DYNAMIC::Offset_Teb = 0;
DECL_SELECTANY ULONG_PTR ML_ETHREAD_DYNAMIC::Offset_CreateTime = 0;


class ML_OBJECT_HEADER_DYNAMIC
{
protected:
    PVOID Object;

    static ULONG_PTR Offset_TypeIndex;

public:
    ML_OBJECT_HEADER_DYNAMIC(PVOID Object = nullptr)
    {
        (*this) = Object;
    }

    ML_OBJECT_HEADER_DYNAMIC* operator->()
    {
        return this;
    }

    ML_OBJECT_HEADER_DYNAMIC& operator=(PVOID Object)
    {
        this->Object = Object;
        return *this;
    }

    PROPERTY(UCHAR, TypeIndex);

    GET(TypeIndex)
    {
        return *(PUCHAR)PtrAdd(this->Object, Offset_TypeIndex);
    }

    SET(TypeIndex)
    {
        _InterlockedExchange8((PCHAR)PtrAdd(this->Object, Offset_TypeIndex), value);
    }

    NoInline static NTSTATUS InitializeOffset()
    {
        BYTE            TypeIndexTable[0x60];
        PBYTE           Buffer, TypeIndexTableEnd;
        ULONG_PTR       TypeIndex;
        ML_OBJECT_TYPE  TypeObjectType;

        ZeroMemory(TypeIndexTable, sizeof(TypeIndexTable));

        TypeIndexTableEnd = &TypeIndexTable[countof(TypeIndexTable)];

        TypeObjectType = ObGetObjectType(*TmResourceManagerObjectType);
        TypeIndex = TypeObjectType->Index;

        for (Buffer = TypeIndexTableEnd - 1; Buffer != TypeIndexTable; --Buffer)
        {
            *Buffer = TypeIndex;
            if (ObGetObjectType(TypeIndexTableEnd) == TypeObjectType)
            {
                Offset_TypeIndex = PtrOffset(Buffer, TypeIndexTableEnd);
                return STATUS_SUCCESS;
            }
            *Buffer = 0;
        }

        return STATUS_UNSUCCESSFUL;
    }
};

DECL_SELECTANY ULONG_PTR ML_OBJECT_HEADER_DYNAMIC::Offset_TypeIndex = 0;


#pragma pop_macro("DEFINE_PROPERTY")
#pragma pop_macro("DECLARE_ACCESSOR")
#pragma pop_macro("FIELD_ACCESSOR")
#pragma pop_macro("SET_ACCESSOR")
#pragma pop_macro("CALL_ACCESSOR")

class StructAccessorManager
{
public:

    NoInline static NTSTATUS Initialize()
    {
        FAIL_RETURN(ML_OBJECT_HEADER_DYNAMIC::InitializeOffset());
        FAIL_RETURN(ML_EPROCESS_DYNAMIC::InitializeOffset());
        FAIL_RETURN(ML_ETHREAD_DYNAMIC::InitializeOffset());

        ML_OBJECT_TYPE ObjectType;

        if (ObjectType->Success == FALSE)
            return STATUS_NOT_SUPPORTED;

        ML_OBJECT_TYPE_INITIALIZER_DATA ObjectTypeInitializer;

        if (ObjectTypeInitializer->Success == FALSE)
            return STATUS_NOT_SUPPORTED;

        return STATUS_SUCCESS;
    }
};

#endif // r0
#endif

#endif // _MYLIBRARY_H_9fb91137_38ac_4d5e_8572_b6e12d23cb15_