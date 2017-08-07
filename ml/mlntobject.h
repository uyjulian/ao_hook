#ifndef _MLNTOBJECT_H_a9853fbd_9aa2_4bfd_8efc_84dfd9ff9679_
#define _MLNTOBJECT_H_a9853fbd_9aa2_4bfd_8efc_84dfd9ff9679_


ML_NAMESPACE_BEGIN(Ob);

#if ML_KERNEL_MODE

NTSTATUS
CreateObjectType(
    IN  PUNICODE_STRING             TypeName,
    IN  POBJECT_TYPE_INITIALIZER    ObjectTypeInitializer,
    OUT POBJECT_TYPE*               ObjectType
);

#endif // r0

ML_NAMESPACE_END_(Ob);

#endif // _MLNTOBJECT_H_a9853fbd_9aa2_4bfd_8efc_84dfd9ff9679_