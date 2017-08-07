#include "mylibrary.h"


ML_NAMESPACE

LONG_PTR MlInitialize()
{
    if (MemoryAllocator::GetGlobalHeap() == NULL && MemoryAllocator::CreateGlobalHeap() == nullptr)
        return STATUS_NO_MEMORY;

    return STATUS_SUCCESS;
}

LONG_PTR MlUnInitialize()
{
    MemoryAllocator::DestroyGlobalHeap();

    return 0;
}

MY_NAMESPACE_END

