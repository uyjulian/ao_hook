#ifndef _POINTER_H_6fac0398_48bd_494d_b268_63d42c5b7dc9_
#define _POINTER_H_6fac0398_48bd_494d_b268_63d42c5b7dc9_


template<typename PtrBase, typename PointerType>
class PointerImpl
{
protected:
    PULongPtr   RefCount;
    PointerType Reference;

protected:
    ULongPtr AddRef()
    {
        return RefCount != NULL ? ++*RefCount : 0;
    }

    ULongPtr Release()
    {
        return ((PtrBase *)this)->ReleaseWorker();
    }

    ULongPtr ReleaseWorker()
    {
        if (RefCount == NULL)
            return 0;

        ULongPtr RefCount = --*this->RefCount;

        if (RefCount == 0)
        {
            delete this->RefCount;

            ((PtrBase *)this)->ReleasePointer();

            RefCount = NULL;
            Reference = NULL;
        }

        return RefCount;
    }

    void ReleasePointer()
    {
        delete Reference;
    }

public:
    PointerImpl()
    {
        RefCount = NULL;
        Reference = NULL;
    }

    PointerImpl(PointerType Ptr)
    {
        (*this) = Ptr;
    }

    PointerImpl(const PointerImpl<PtrBase, PointerType>& Ptr)
    {
        (*this) = Ptr;
    }

    ~PointerImpl()
    {
        Release();
    }

    PtrBase& operator=(PointerType Ptr)
    {
        if (Ptr == Reference)
            return *((PtrBase *)this);

        Release();

        if (Ptr == NULL)
        {
            RefCount = NULL;
            Reference = NULL;
        }
        else
        {
            RefCount = new ULongPtr(1);
            Reference = Ptr;
        }

        return *((PtrBase *)this);
    }

    PtrBase& operator=(const PtrBase& Ptr)
    {
        if (this == &Ptr)
            return *((PtrBase *)this);

        Release();

        RefCount = Ptr.RefCount;
        Reference = Ptr.Reference;

        AddRef();

        return *((PtrBase *)this);
    }

    Bool operator== (const PointerType Ptr) const
    {
        return Reference == Ptr;
    }

    operator void*() const
    {
        return Reference;
    }

    Bool operator!= (const PointerType Ptr) const
    {
        return Reference != Ptr;
    }

    Bool operator> (const PointerType Ptr) const
    {
        return Reference > Ptr;
    }

    Bool operator< (const PointerType Ptr) const
    {
        return Reference < Ptr;
    }

    Bool operator>= (const PointerType Ptr) const
    {
        return Reference >= Ptr;
    }

    Bool operator<= (const PointerType Ptr) const
    {
        return Reference <= Ptr;
    }

    Bool operator!() const
    {
        return Reference != NULL;
    }

    operator bool() const
    {
        return Reference != NULL;
    }

    PointerType operator-> () const
    {
        return Reference;
    }
};

template<typename PointerType>
class Pointer : public PointerImpl<Pointer<PointerType>, PointerType>
{
public:
    const TYPE_OF(*((PointerType)0))& operator[](Int Index) const
    {
        return Reference[Index];
    }

    Pointer<PointerType>& operator=(PointerType Ptr)
    {
        return __super::operator=(Ptr);
    }

    Pointer<PointerType>& operator=(const Pointer<PointerType>& Ptr)
    {
        return __super::operator=(Ptr);
    }
};

#if ML_USER_MODE

#endif // r3

#endif // _POINTER_H_6fac0398_48bd_494d_b268_63d42c5b7dc9_