#include "mylibrary.h"

ML_NAMESPACE

ML_NAMESPACE_BEGIN(Native)


ML_NAMESPACE_BEGIN(Ob)

#if ML_KERNEL_MODE

NTSTATUS
CreateObjectType(
    IN  PUNICODE_STRING             TypeName,
    IN  POBJECT_TYPE_INITIALIZER    ObjectTypeInitializer,
    OUT POBJECT_TYPE*               ObjectType
)
{
    NTSTATUS Status;

    *ObjectType = nullptr;

    Status = ObCreateObjectType(TypeName, ObjectTypeInitializer, nullptr, ObjectType);
    if (Status != STATUS_OBJECT_NAME_COLLISION)
        return Status;

    BYTE ObjectHack[0x100];
    POBJECT_TYPE Type;
    PVOID Object;
    ML_OBJECT_HEADER_DYNAMIC ObjectHeader;

    ZeroMemory(ObjectHack, sizeof(ObjectHack));

    Object = &ObjectHack[countof(ObjectHack) / 2];
    ObjectHeader = Object;

    for (ULONG_PTR TypeIndex = ML_OBJECT_TYPE(ObGetObjectType(*TmResourceManagerObjectType))->Index; TypeIndex != 0x100; ++TypeIndex)
    {
        ObjectHeader->TypeIndex = TypeIndex;
        Type = ObGetObjectType(Object);
        if (Type == nullptr || (ULONG_PTR)Type == 1)
            continue;

        if (RtlEqualUnicodeString(&ML_OBJECT_TYPE(Type)->Name, TypeName, TRUE) == FALSE)
            continue;

        *ObjectType = Type;

        return STATUS_SUCCESS;
    }

    return Status;
}

#endif

ML_NAMESPACE_END_(Ob);


ML_NAMESPACE_END_(Native)

ML_NAMESPACE_END
