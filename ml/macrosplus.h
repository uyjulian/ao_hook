#ifndef _MACROSPLUS_H_86ea4111_75e2_4619_8573_5f8575474b66_
#define _MACROSPLUS_H_86ea4111_75e2_4619_8573_5f8575474b66_


#if CPP_DEFINED

template<class T>
inline void SafeReleaseT(T*& Object)
{
    if (Object != NULL)
    {
        Object->Release();
        Object = NULL;
    }
}

template<class T>
inline void SafeDeleteT(T*& Object)
{
    if (Object != NULL)
    {
        delete Object;
        Object = NULL;
    }
}

template<class T>
inline void SafeDeleteArrayT(T*& Object)
{
    if (Object != NULL)
    {
        delete[] Object;
        Object = NULL;
    }
}

/************************************************************************
  bsearch
************************************************************************/

#define BinarySearchM(ValuePtr, Value, Context) [&] (auto ValuePtr, auto Value, auto Context)

template<class ArgType, class ValueType, class RoutineContext, class CompareRoutine>
ArgType
BinarySearch(
    ArgType             First,
    ArgType             Last,
    const ValueType&    Value,
    CompareRoutine      CompareFunction,
    RoutineContext      Context = NULL
)
{
    ULONG_PTR   Left, Right, Middle;
    LONG_PTR    Result;

    if (First == Last)
        return CompareFunction(&First[0], Value, Context) == 0 ? First : NULL;

    Left    = 0;
    Right   = Last - First - 1;

    Result = CompareFunction(&First[0], Value, Context);
    if (Result > 0)
        return NULL;

    if (Result == 0)
        return First;

    Result = CompareFunction(&Last[-1], Value, Context);
    if (Result < 0)
        return NULL;

    if (Result == 0)
        return &Last[-1];

    while (Left < Right)
    {
        Middle = (Right - Left) / 2 + Left;

        Result = CompareFunction(&First[Middle], Value, Context);
        if (Result == 0)
            return &First[Middle];

        if (Result < 0)
        {
            Left = Middle + 1;
        }
        else
        {
            Right = Middle - 1;
        }
    }

    Result = CompareFunction(&First[Left], Value, Context);

    return Result == 0 ? &First[Left] : NULL;
}

/************************************************************************
  type_of
  from boost
************************************************************************/

#define MY_STATIC_CONSTANT(type, seq) static const type seq

# if (_MSC_VER >= 1300) && defined(_MSC_EXTENSIONS)
        template<int N> struct the_counter;

        template<typename T,int N = 5/*for similarity*/>
        struct encode_counter
        {
            __if_exists(the_counter<N + 256>)
            {
                MY_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 257>::count));
            }
            __if_not_exists(the_counter<N + 256>)
            {
                __if_exists(the_counter<N + 64>)
                {
                    MY_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 65>::count));
                }
                __if_not_exists(the_counter<N + 64>)
                {
                    __if_exists(the_counter<N + 16>)
                    {
                        MY_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 17>::count));
                    }
                    __if_not_exists(the_counter<N + 16>)
                    {
                        __if_exists(the_counter<N + 4>)
                        {
                            MY_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 5>::count));
                        }
                        __if_not_exists(the_counter<N + 4>)
                        {
                            __if_exists(the_counter<N>)
                            {
                                MY_STATIC_CONSTANT(unsigned,count=(encode_counter<T,N + 1>::count));
                            }
                            __if_not_exists(the_counter<N>)
                            {
                                MY_STATIC_CONSTANT(unsigned,count=N);
                                typedef the_counter<N> type;
                            }
                        }
                    }
                }
            }
        };

# define MY_TYPEOF_INDEX(T) (encode_counter<T>::count)
# define MY_TYPEOF_NEXT_INDEX(next)
# else
        template<int N> struct encode_counter : encode_counter<N - 1> {};
        template<> struct encode_counter<0> {};

        //Need to default to a larger value than 4, as due to MSVC's ETI errors. (sizeof(int)==4)
        char (*encode_index(...))[5];

# define MY_TYPEOF_INDEX(T) (sizeof(*encode_index((encode_counter<1005>*)0)))
# define MY_TYPEOF_NEXT_INDEX(next) friend char (*encode_index(encode_counter<next>*))[next];
# endif

        //Typeof code

# if _MSC_VER == 1300
        template<typename ID>
        struct msvc_extract_type
        {
            template<bool>
            struct id2type_impl;

            typedef id2type_impl<true> id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID>
        {
            template<>
            struct id2type_impl<true>  //VC7.0 specific bugfeature
            {
                typedef T type;
            };
        };
#elif _MSC_VER >= 1400
        struct msvc_extract_type_default_param {};

        template<typename ID, typename T = msvc_extract_type_default_param>
        struct msvc_extract_type;

        template<typename ID>
        struct msvc_extract_type<ID, msvc_extract_type_default_param> {
            template<bool>
            struct id2type_impl;

            typedef id2type_impl<true> id2type;
        };

        template<typename ID, typename T>
        struct msvc_extract_type : msvc_extract_type<ID,msvc_extract_type_default_param>
        {
            template<>
            struct id2type_impl<true>  //VC8.0 specific bugfeature
            {
                typedef T type;
            };
            template<bool>
            struct id2type_impl;

            typedef id2type_impl<true> id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID, T>
        {
        };
# else
        template<typename ID>
        struct msvc_extract_type
        {
            struct id2type;
        };

        template<typename T, typename ID>
        struct msvc_register_type : msvc_extract_type<ID>
        {
            typedef msvc_extract_type<ID> base_type;
            struct base_type::id2type // This uses nice VC6.5 and VC7.1 bugfeature
            {
                typedef T type;
            };
        };
# endif

        template<int N> struct int_;

        template<int ID>
        struct msvc_typeid_wrapper {
            typedef typename msvc_extract_type<int_<ID> >::id2type id2type;
            typedef typename id2type::type type;
        };
        //Workaround for ETI-bug for VC6 and VC7
        template<>
        struct msvc_typeid_wrapper<1> {
            typedef msvc_typeid_wrapper<1> type;
        };
        //Workaround for ETI-bug for VC7.1
        template<>
        struct msvc_typeid_wrapper<4> {
            typedef msvc_typeid_wrapper<4> type;
        };

        //Tie it all together
        template<typename T>
        struct encode_type
        {
            //Get the next available compile time constants index
            MY_STATIC_CONSTANT(unsigned,value=MY_TYPEOF_INDEX(T));
            //Instantiate the template
            typedef typename msvc_register_type<T,int_<value> >::id2type type;
            //Set the next compile time constants index
            MY_STATIC_CONSTANT(unsigned,next=value+1);
            //Increment the compile time constant (only needed when extensions are not active
            MY_TYPEOF_NEXT_INDEX(next);
        };

        template<class T>
        struct sizer
        {
            typedef char(*type)[encode_type<T>::value];
        };
# if 0 && (_MSC_VER >= 1310)
        template<typename T> typename disable_if<
            typename is_function<T>::type,
            typename sizer<T>::type>::type encode_start(T const&);

        template<typename T> typename enable_if<
            typename is_function<T>::type,
            typename sizer<T>::type>::type encode_start(T&);
# else
        template<typename T>
            typename sizer<T>::type encode_start(T const&);
# endif
        template<typename Organizer, typename T>
        msvc_register_type<T,Organizer> typeof_register_type(const T&,Organizer* =0);

#if _MSC_VER < 1500

#define TYPE_OF(expr) \
    msvc_typeid_wrapper<sizeof(*encode_start(expr))>::type

#else

#define TYPE_OF decltype

#endif

#define FUNC_POINTER(__func) TYPE_OF(__func)*
#define API_POINTER(__func) TYPE_OF(&__func)
#define PTR_OF(_var) TYPE_OF(_var)*


/************************************************************************
  scope exit
************************************************************************/
#pragma warning(disable:4512)

template<typename Lambda>
struct LambdaWrapper
{
    const Lambda &fn;

    ForceInline LambdaWrapper(const Lambda &fn) : fn(fn)
    {
        ;
    }

    ForceInline void operator()()
    {
        fn();
    }

    ForceInline ~LambdaWrapper()
    {
        fn();
    }
};

#pragma warning(default:4512)

template<typename ScopeExitRoutine>
ForceInline
LambdaWrapper<ScopeExitRoutine>
CreateScopeExitObject(
    const ScopeExitRoutine &ScopeExitFunction
)
{
    return LambdaWrapper<ScopeExitRoutine>(ScopeExitFunction);
}

#define SCOPE_EXIT auto MAKE_NAME_PREFIX(scope_exit_obj_, __LINE__) = CreateScopeExitObject([&] ()
#define SCOPE_EXIT_END )



template<class InIterator, class Function>
ForceInline Function ForEachZ(InIterator _Count, Function _Func)
{
    for (; _Count ; --_Count)
        _Func(_Count);

    return (_Func);
}

template<class InIterator, class Function>
ForceInline Function ForEachR(InIterator _First, InIterator _Last, Function _Func)
{
    for (; _First != _Last; --_First)
        _Func(_First);

    return (_Func);
}

template<class InIterator, class Function>
ForceInline Function ForEach(InIterator _First, InIterator _Last, Function _Func)
{
    for (; _First != _Last; ++_First)
        _Func(_First);

    return (_Func);
}

template<ULong_Ptr StringLength>
struct HashStringConst
{
    template<class StringType> static ULong Hash(StringType String, ULong Initial = 0)
    {
        Initial = _rotl(Initial, 0xD) ^ (Byte)(String[0]);
        return HashStringConst<StringLength - 1>::Hash(++String, Initial);
    }
};

template<>
struct HashStringConst<0>
{
    template<class StringType> static ULong Hash(StringType String, ULong Initial = 0)
    {
        return Initial;
    }
};

#define CONST_STRHASH(_str) HashStringConst<CONST_STRLEN((_str))>::Hash(_str)

#endif // CPP_DEFINED

#endif // _MACROSPLUS_H_86ea4111_75e2_4619_8573_5f8575474b66_
