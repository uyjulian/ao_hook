#ifndef _TRAITS_H_0c85514c_9b05_4eaf_a9ae_8e46a810207c_
#define _TRAITS_H_0c85514c_9b05_4eaf_a9ae_8e46a810207c_

template<typename T>
struct TypeTraits
{
    typedef T           VALUE_TYPE;
    typedef T&          REF_TYPE;
    typedef const T&    CONST_REF_TYPE;
    VALUE_TYPE          value;
};

template<typename T>
struct TypeTraits<T&>
{
    typedef T           VALUE_TYPE;
    typedef T&          REF_TYPE;
    typedef const T&    CONST_REF_TYPE;
    VALUE_TYPE          value;
};

template<typename T>
struct TypeTraits<const T&>
{
    typedef T           VALUE_TYPE;
    typedef T&          REF_TYPE;
    typedef const T&    CONST_REF_TYPE;
    VALUE_TYPE          value;
};

template<>
struct TypeTraits<VOID>
{
    typedef VOID VALUE_TYPE;
    typedef VOID REF_TYPE;
    typedef VOID CONST_REF_TYPE;
};

#endif // _TRAITS_H_0c85514c_9b05_4eaf_a9ae_8e46a810207c_