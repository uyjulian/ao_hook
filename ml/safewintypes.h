#ifndef _SAFEWINTYPES_H_1bcf869c_5f0d_4e2b_9b04_212129cd7897_
#define _SAFEWINTYPES_H_1bcf869c_5f0d_4e2b_9b04_212129cd7897_


template<>
class Pointer<HANDLE> : public PointerImpl<Pointer<HANDLE>, HANDLE>
{
    friend class PointerImpl<Pointer<HANDLE>, HANDLE>;

    void ReleasePointer()
    {
        switch ((ULONG_PTR)Reference)
        {
            case (ULONG_PTR)NtCurrentProcess():
            case (ULONG_PTR)NtCurrentThread():
                return;
        }

        ZwClose(Reference);
    }

public:
    Pointer<HANDLE>& operator=(HANDLE Ptr)
    {
        return __super::operator=(Ptr);
    }

    Pointer<HANDLE>& operator=(const Pointer<HANDLE>& Ptr)
    {
        return __super::operator=(Ptr);
    }
};

typedef Pointer<HANDLE> Handle;

#if ML_USER_MODE

class FindHandle : public PointerImpl<FindHandle, HANDLE>
{
    friend class PointerImpl<FindHandle, HANDLE>;

    void ReleasePointer()
    {
        FindClose(Reference);
    }

public:
    FindHandle& operator=(HANDLE Ptr)
    {
        return __super::operator=(Ptr);
    }

    FindHandle& operator=(const FindHandle& Ptr)
    {
        return __super::operator=(Ptr);
    }
};

#endif // r3

#endif // _SAFEWINTYPES_H_1bcf869c_5f0d_4e2b_9b04_212129cd7897_