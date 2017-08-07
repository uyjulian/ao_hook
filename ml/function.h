#ifndef _FUNCTION_H_a0ede8da_ec11_4122_bf7f_3b39ea104800_
#define _FUNCTION_H_a0ede8da_ec11_4122_bf7f_3b39ea104800_


template<typename T> class Function;

#pragma warning(push)
#pragma warning(disable:4510 4610)

template<typename T> class Function : public Function<TYPE_OF(&T::operator())> {};

#pragma warning(pop)

#pragma push_macro("DEFINE_FUNCTION_CLASS")

#undef DEFINE_FUNCTION_CLASS
#define DEFINE_FUNCTION_CLASS(call_convention)\
    template<typename R, typename... ARGS> \
    class Function<R call_convention(ARGS...)> \
    { \
    public: \
        typedef Function<R call_convention(ARGS...)> SELF_TYPE; \
        typedef R RET_TYPE; \
        static const ULONG_PTR NumberOfArguments = sizeof...(ARGS); \
        typedef R (FUNCTION_TYPE)(ARGS...); \
\
        class Invoker \
        { \
            ULONG_PTR RefCount; \
        public: \
            Invoker() : RefCount(1) {} \
            virtual ~Invoker() {} \
            virtual R Invoke(ARGS...) = 0; \
\
            void AddRef() \
            { \
                ++this->RefCount; \
            } \
\
            void Release() \
            { \
                if (--this->RefCount == 0) \
                    delete this; \
            } \
\
        }; \
\
        template<typename F> \
        class FunctionInvoker : public Invoker \
        { \
        protected: \
            F func; \
\
        public: \
            FunctionInvoker(const F& function) : func(function) \
            { \
            } \
\
            R Invoke(ARGS... args) \
            { \
                return func(args...); \
            } \
        }; \
\
        Invoker *invoker; \
\
        NoInline Function(const SELF_TYPE &func) \
        { \
            this->invoker = func.invoker; \
            this->invoker->AddRef(); \
        } \
\
        template<typename F> \
        NoInline Function(const F &func) \
        { \
            this->invoker = new FunctionInvoker<F>(func); \
        } \
\
        template<typename F> \
        SELF_TYPE& operator=(const F &func) \
        { \
            this->~Function(); \
            this->invoker = new FunctionInvoker<F>(func); \
            return *this; \
        } \
\
        NoInline ~Function() \
        { \
            if (invoker != nullptr) \
                invoker->Release(); \
        } \
\
        R operator()(ARGS... args) \
        { \
            return this->invoker->Invoke(args...); \
        } \
    }

#define DEFINE_FUNCTION_POINTER_CLASS(call_convention)\
    template<typename R, typename... ARGS> \
    class Function<R (call_convention*)(ARGS...)> \
    { \
    public: \
        typedef Function<R call_convention(ARGS...)> SELF_TYPE; \
        typedef R RET_TYPE; \
        static const ULONG_PTR NumberOfArguments = sizeof...(ARGS); \
        typedef R (FUNCTION_TYPE)(ARGS...); \
\
        class Invoker \
        { \
            ULONG_PTR RefCount; \
        public: \
            Invoker() : RefCount(1) {} \
            virtual ~Invoker() {} \
            virtual R Invoke(ARGS...) = 0; \
\
            void AddRef() \
            { \
                ++this->RefCount; \
            } \
\
            void Release() \
            { \
                if (--this->RefCount == 0) \
                    delete this; \
            } \
\
        }; \
\
        template<typename F> \
        class FunctionInvoker : public Invoker \
        { \
        protected: \
            F func; \
\
        public: \
            FunctionInvoker(const F& function) : func(function) \
            { \
            } \
\
            R Invoke(ARGS... args) \
            { \
                return func(args...); \
            } \
        }; \
\
        Invoker *invoker; \
\
        NoInline Function(const SELF_TYPE &func) \
        { \
            this->invoker = func.invoker; \
            this->invoker->AddRef(); \
        } \
\
        template<typename F> \
        NoInline Function(const F &func) \
        { \
            this->invoker = new FunctionInvoker<F>(func); \
        } \
\
        template<typename F> \
        SELF_TYPE& operator=(const F &func) \
        { \
            this->~Function(); \
            this->invoker = new FunctionInvoker<F>(func); \
            return *this; \
        } \
\
        NoInline ~Function() \
        { \
            if (invoker != nullptr) \
                invoker->Release(); \
        } \
\
        R operator()(ARGS... args) \
        { \
            return this->invoker->Invoke(args...); \
        } \
    };

#define DEFINE_CLASS_METHOD_CLASS(...) \
    template<typename CLASS, typename R, typename... ARGS> \
    class Function<R(__VA_ARGS__ CLASS::*)(const CLASS&, ARGS...)> \
    { \
    public: \
        typedef Function<R __VA_ARGS__(const CLASS&, ARGS...)> SELF_TYPE; \
        typedef R RET_TYPE; \
        static const ULONG_PTR NumberOfArguments = sizeof...(ARGS); \
        typedef R (FUNCTION_TYPE)(const CLASS&, ARGS...); \
\
        class Invoker \
        { \
            ULONG_PTR RefCount; \
        public: \
            Invoker() : RefCount(1) {} \
            virtual ~Invoker() {} \
            virtual R Invoke(const CLASS&, ARGS...) = 0; \
\
            void AddRef() \
            { \
                ++this->RefCount; \
            } \
\
            void Release() \
            { \
                if (--this->RefCount == 0) \
                    delete this; \
            } \
\
        }; \
\
        template<typename F> \
        class FunctionInvoker : public Invoker \
        { \
        protected: \
            F func; \
\
        public: \
            FunctionInvoker(const F& function) : func(function) \
            { \
            } \
\
            R Invoke(const CLASS& thiz, ARGS... args) \
            { \
                return func(thiz, args...); \
            } \
        }; \
\
        Invoker *invoker; \
\
        NoInline Function(const SELF_TYPE &func) \
        { \
            this->invoker = func.invoker; \
            this->invoker->AddRef(); \
        } \
\
        template<typename F> \
        NoInline Function(const F &func) \
        { \
            this->invoker = new FunctionInvoker<F>(func); \
        } \
\
        template<typename F> \
        SELF_TYPE& operator=(const F &func) \
        { \
            this->~Function(); \
            this->invoker = new FunctionInvoker<F>(func); \
            return *this; \
        } \
\
        NoInline ~Function() \
        { \
            if (invoker != nullptr) \
                invoker->Release(); \
        } \
\
        R operator()(const CLASS& thiz, ARGS... args) \
        { \
            return this->invoker->Invoke(thiz, args...); \
        } \
    };


#define DEFINE_LAMBDA_CLASS(...) \
    template<typename CLASS, typename R, typename... ARGS> \
    class Function<R(__VA_ARGS__ CLASS::*)(ARGS...) const> \
    { \
    public: \
        typedef Function<R __VA_ARGS__(ARGS...)> SELF_TYPE; \
        typedef R RET_TYPE; \
        static const ULONG_PTR NumberOfArguments = sizeof...(ARGS); \
        typedef R (FUNCTION_TYPE)(ARGS...); \
\
        class Invoker \
        { \
            ULONG_PTR RefCount; \
        public: \
            Invoker() : RefCount(1) {} \
            virtual ~Invoker() {} \
            virtual R Invoke(ARGS...) = 0; \
\
            void AddRef() \
            { \
                ++this->RefCount; \
            } \
\
            void Release() \
            { \
                if (--this->RefCount == 0) \
                    delete this; \
            } \
\
        }; \
\
        template<typename F> \
        class FunctionInvoker : public Invoker \
        { \
        protected: \
            F func; \
\
        public: \
            FunctionInvoker(const F& function) : func(function) \
            { \
            } \
\
            R Invoke(ARGS... args) \
            { \
                return func(args...); \
            } \
        }; \
\
        Invoker *invoker; \
\
        NoInline Function(const SELF_TYPE &func) \
        { \
            this->invoker = func.invoker; \
            this->invoker->AddRef(); \
        } \
\
        template<typename F> \
        NoInline Function(const F &func) \
        { \
            this->invoker = new FunctionInvoker<F>(func); \
        } \
\
        template<typename F> \
        SELF_TYPE& operator=(const F &func) \
        { \
            this->~Function(); \
            this->invoker = new FunctionInvoker<F>(func); \
            return *this; \
        } \
\
        NoInline ~Function() \
        { \
            if (invoker != nullptr) \
                invoker->Release(); \
        } \
\
        R operator()(ARGS... args) \
        { \
            return this->invoker->Invoke(args...); \
        } \
    };


#if ML_X86

DEFINE_FUNCTION_CLASS(__stdcall);
DEFINE_FUNCTION_CLASS(__cdecl);

DEFINE_FUNCTION_POINTER_CLASS(__stdcall);
DEFINE_FUNCTION_POINTER_CLASS(__cdecl);

// DEFINE_CLASS_METHOD_CLASS(__stdcall);
// DEFINE_CLASS_METHOD_CLASS(__cdecl);
// DEFINE_CLASS_METHOD_CLASS();

DEFINE_LAMBDA_CLASS(__stdcall);
DEFINE_LAMBDA_CLASS(__cdecl);
DEFINE_LAMBDA_CLASS();

#if !CPP_CLI_DEFINED

DEFINE_FUNCTION_CLASS(__fastcall);
DEFINE_FUNCTION_POINTER_CLASS(__fastcall);

// DEFINE_CLASS_METHOD_CLASS(__fastcall);
DEFINE_LAMBDA_CLASS(__fastcall);

#endif // c++/cli

#elif ML_AMD64

DEFINE_FUNCTION_CLASS(__cdecl);
DEFINE_FUNCTION_POINTER_CLASS(__cdecl);

// DEFINE_CLASS_METHOD_CLASS(__cdecl);
DEFINE_LAMBDA_CLASS(__cdecl);

#endif

#pragma pop_macro("DEFINE_FUNCTION_CLASS")

#endif // _FUNCTION_H_a0ede8da_ec11_4122_bf7f_3b39ea104800_