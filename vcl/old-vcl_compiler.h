//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_compiler_h_
#define vcl_compiler_h_

// ** First declare the defaults and describe them, then fix individual
// compilers.  Examples of each construct are provided, for those who don't keep
// fully abreast of the latest C++ buzzwords.

//--------------------------------------------------------------------------------

#include <vxl_compiler_config.h>

// -- VCL_USE_NATIVE_COMPLEX
// Use the compiler's complex. Set from VCL_USE_NATIVE_STL at the moment.
// If you change it, test on 7 platforms, with all 4 combinations of
// USE_NATIVE_STL, USE_NATIVE_COMPLEX
#define VCL_USE_NATIVE_COMPLEX VCL_USE_NATIVE_STL

// -- VCL_COMMA
//
// Handy for passing things with commas in them to CPP macros.  e.g. 
// DO_MACRO(pair<A,B>) can be replaced by DO_MACRO(pair<A VCL_COMMA B>).
#define VCL_COMMA ,

// -- VCL_VOID_RETURN
//
// VCL_VOID_RETURN is used as a return type where void is expected,
// as in return VCL_VOID_RETURN ;
#define VCL_VOID_RETURN /*empty*/

// -- VCL_HAS_DYNAMIC_CAST
// 
// True if the compiler supports dynamic cast.
//
//#define VCL_HAS_DYNAMIC_CAST 1


// -- VCL_STATIC_CONST_INIT_INT:
// 
// ANSI allows
//     class A {
//     	 static const int x = 27;
//     };
// And there is a speed advantage, so we want to use it where supported.
// The macro is used like this:
//       static const int x VCL_STATIC_CONST_INIT_INT(27);
//#define VCL_STATIC_CONST_INIT_INT(x) = x


// -- VCL_STATIC_CONST_INIT_FLOAT:
// 
// GCC allows the above, but with floating point types, ANSI doesn't.
// Again, we'll use it if we've got it.
//#define VCL_STATIC_CONST_INIT_FLOAT(x)


// -- VCL_IMPLEMENT_STATIC_CONSTS:
// 
// True if static consts must be defined in some source file.  I don't known
// what ANSI has to say about this, but anyway, the above example needs this
// in some .C file:
//     #if VCL_IMPLEMENT_STATIC_CONSTS
//     const int A::x = 27;
//     #endif
///#define VCL_IMPLEMENT_STATIC_CONSTS 1


// -- VCL_FOR_SCOPE_HACK:
// 
// True if the compiler uses old-style 'for' loop scoping.
// Setting this nonzero causes the Henderson trick to be used.
//#define VCL_FOR_SCOPE_HACK 0



// ** Various template issues.

// -- VCL_HAS_MEMBER_TEMPLATES:
// 
// True if the compiler supports template members of template classes.  e.g.
//     template <class U> class A {
//     	 template <class V> void f(V);
//     }
//#define VCL_HAS_MEMBER_TEMPLATES 1


// -- VCL_CAN_DO_PARTIAL_SPECIALIZATION
// 
// True if the compiler supports partial specializations of templates. e.g.
// template <class T>
// class vector<T*> : public vector<void *> { .. inline methods .. };
//
//#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0


// -- VCL_INSTANTIATE_INLINE:
// 
// Use this to instantiate inline functions for compilers that need it. 
// If a non-member template function is declared inline e.g.
//     template <class T>
//     inline T max(T a, T b) { return ....; }
// Then instantiate using
//     VCL_INSTANTIATE_INLINE(T max(T,T))
//#define VCL_INSTANTIATE_INLINE(fn_decl) template fn_decl ;


// -- VCL_DO_NOT_INSTANTIATE:
//
// If a method is defined on some template, but makes no sense for some
// instances of that template, the compiler should not complain unless the
// method is actually used.  For example
//     template <class T>
//     class T {
//    	 int bad_method() {
//    	   return T::f();  // Requires T to have static method f
//    	 }
//     };
//
// The language allows you to use a T<int> even though int::f() is garbage,
// *providing* you never call T.bad_method().
//
// Most compilers don't implement that yet, so the solution is to provide a
// dummy specialization of T::bad_method that returns something mundane and
// stops the standard bad_method from being generated.  For this, use:
//     VCL_DO_NOT_INSTANTIATE(int T::bad_method(), some_return_value)
// if the function is void, use VCL_VOID_RETURN as the return value
//#define VCL_DO_NOT_INSTANTIATE(text,ret)


// -- VCL_UNINSTANTIATE_SPECIALIZATION:
// 
// OK, various compilers do various silly things about instantiation of
// functions/methods that have been specialized.  Use this macro to tell
// the compiler not to generate code for methods which have been specialized
//      VCL_UNINSTANTIATE_SPECIALIZATION(int T::specialized_method())
// It should be placed after the "template class A<T>;"
//#define VCL_UNINSTANTIATE_SPECIALIZATION(x)

// -- VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION:
// 
// gcc is sensible about specializations if it has seen the definition,
// but if it's in another file, need to use extern to tell it.
//      VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(int T::specialized_method())
// It should be placed before the "template class A<T>;"
//#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)


// -- VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER
//
// Some compilers require you to place static template members in a .C file.
// Put them using
//
//      VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(int A<int>::var = 0)
// before the template class A<int>; with
//      VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(A<int>::var)
// afterwards
//
//#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;


// -- VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
//
// Either 0 or 1
//
// On some compilers (in particular gcc 2.7.2.3), the compiler doesn't
// know how to cast a templated derived class to a templated base class
// (eg. vnl_matrix_fixed<3,3,double> -> vnl_matrix<double>) when doing overload 
// resolution. Making the overloaded function a friend of the class makes
// the problem go away.


// -- VCL_OVERLOAD_CAST
//
// Some compilers (gcc 2.7.2.3 and SGI native 6.0) often won't perform
// certain implicit casts. E.g. casting a templated derived class to a
// templated base class (see above), or even realizing that
// "template void foo(float const * const *, float * const *, int, int)"
// can be called with parameters of type "(float **, float **, int, int)".
//   To fix the code, it is tempting to add an explicit cast and get on
// with things, but that would throw away the checking performed by more
// helpful compilers. Use VCL_OVERLOAD_CAST instead.
//
//#define VCL_OVERLOAD_CAST(T, x) ((T)(x))
//#define VCL_OVERLOAD_CAST(T, x) (x)


// -- VCL_DECLARE_SPECIALIZATION
//
// On EGCS 1.1, even with -fguiding-decls, function template specializations
// need to be properly forward declared.  This means the declaration must be
// visible before it is used, and must be preceded by "template <>".
// The symptom of not doing so is that specializations are ignored.
// Use VCL_DECLARE_SPECIALIZATION just before any specializations to remain
// compatible with older compilers
// *** the use of this macro is deprecated ***

// -- VCL_DEFINE_SPECIALIZATION
// 
// In order to *define* a template (function or class) specialization, the
// definition must be preceded by "template <>" on ISO-conforming compilers.
// Some compilers (eg gcc 2.7.2) make no distinction between an instance
// of a templated function and a function with the same name and signature,
// and so do not support the use of "template <>". Use VCL_DEFINE_SPECIALIZATION
// instead.
// Note that you DO NOT need to forward declare a specialization. E.g. if 
// foo.h says "template <class T> void foo(T *);" and foo.cxx specializes
// void foo<int>(int *), the client doesn't need to know that the template
// symbol he links against is a specialization.

// -- VCL_STL_NULL_TMPL_ARGS
//
// Define to <> for compilers that require them in friend template function
// declarations (i.e., EGCS).
//
//#define VCL_STL_NULL_TMPL_ARGS <>

// -- VCL_DEFAULT_VALUE(x)
//
// Used to provide default values for function args in definition
// Some compilers (GCC272) require defaults in template function definitions
// Other compilers (VC50) disallow defaults in both decls and defs
//#define VCL_DEFAULT_VALUE(x) =x

// -- VCL_NO_STATIC_DATA_MEMBERS
//
// Set if compiler does not support static data members in template classes.
// Uses value determined for STL
#if (__STL_STATIC_TEMPLATE_DATA < 1)
#define VCL_NO_STATIC_DATA_MEMBERS 1
#endif

// -- VCL_DFL_TYPE_PARAM_STLDECL
// -- VCL_DFL_TMPL_PARAM_STLDECL
// EGCS doesn't like definition of default types, viz:
//   template <class A = default> class vector;
//   template <class A = default> class vector { ... };
// This macro is used to say "define if not previously defined, like
//   template <VCL_DFL_TYPE_PARAM_STLDECL(A,a)> class vector { ... };
//
//#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) __DFL_TYPE_PARAM(A,a)
//#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) __STL_DFL_TMPL_PARAM(A,a)

// -- VCL_DFL_TMPL_ARG
// Similarly, when instantiating a templated class with a default template
// argument, some compilers don't like the redeclaration of that argument,
// while others insist on it.
// In such cases, specify the default argument as follows:
//   template class vector <int VCL_DFL_TMPL_ARG(default_iterator) >;
// (Note the missing comma after int: it is inside the macro.)

//#define VCL_DFL_TMPL_ARG(classname) __DFL_TMPL_ARG(classname)

// -- VCL_SUNPRO_ALLOCATOR_HACK
// Nice one.  Can't use vector<T> in a class on SunPro 5, must use
// vector<T, allocator<T> >.  Of course, we cannot expect that other compilers
// call the default allocator allocator<T>, so we must use a macro.  I could
// call it something generic, like VCL_DEFAULT_ALLOCATOR, but to be honest, it's
// a sunpro problem, they deserve the blame.
// Usage:
//    vector<VCL_SUNPRO_ALLOCATOR_HACK(T)>
// #define VCL_SUNPRO_ALLOCATOR_HACK(T) T

// -----------------------------------------------------------------------------

// It's much better to determine the compiler automatically here than to depend
// on command-line flags being set.

#if defined(__sgi) && !defined(__GNUC__)
#  ifndef _COMPILER_VERSION
#     define VCL_SGI_CC_6
#  else
#   if (_COMPILER_VERSION >= 700)
#     define VCL_SGI_CC_7
#   else
#     define VCL_SGI_CC_6
#   endif
#   define VCL_SGI_CC
#  endif
#endif

#if defined(__SUNPRO_CC)
#  define VCL_SUNPRO_CC
#  if (__SUNPRO_CC>=0x500)
#    define VCL_SUNPRO_CC_50
#  else
#    undef VCL_SUNPRO_CC_50
#  endif
#  ifdef INSTANTIATE_TEMPLATES
#    define _RWSTD_COMPILE_INSTANTIATE
#  endif
#endif

#if defined(__GNUC__)
#  define VCL_GCC
#  if (__GNUC__<=1)
#    error "forget it."
#  elif (__GNUC__==2)
#    if (__GNUC_MINOR__>=100)
#      error "I need some help here."
#    elif (__GNUC_MINOR__>=95)
#      define VCL_GCC_295
#    elif (__GNUC_MINOR__>8)
#      define VCL_EGCS
#    elif (__GNUC_MINOR__>7)
#      define VCL_GCC_28
#    elif (__GNUC_MINOR__>6)
#      define VCL_GCC_27
#    endif
#    if (__GNUC_MINOR__>7)
#      define VCL_GCC_EGCS // so this is the union of EGCS, GCC_28 and GCC_295
#    endif
#  else
#    define VCL_GCC_30
#  endif
#  if defined (GNU_LIBSTDCXX_V3)
#    define VCL_GCC_WITH_GNU_LIBSTDCXX_V3
#  else
#    define VCL_GCC_WITH_GNU_LIBSTDCXX_V2
#  endif
#endif

#if defined(_WIN32) || defined(WIN32)
#  define VCL_WIN32
#  if defined(_MSC_VER)
#   if _MSC_VER >= 1200
#    define VCL_VC60 1
#   else
#    define VCL_VC50 1
#   endif
#  endif
#endif

// -----------------------------------------------------------------------------
// work-around to get template instantiation to work correctly with SunPro
// check flag to turn on inlining
#undef IUEi_STL_INLINE
#if defined(INLINE_EXPLICIT_FLAG) && defined(VCL_SUNPRO_CC) && defined(INSTANTIATE_TEMPLATES)
# define IUEi_STL_INLINE
#else
# define IUEi_STL_INLINE inline
#endif

// gcc
#ifdef VCL_GCC_27
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_MEMBER_TEMPLATES 0
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0
#define VCL_HAS_DYNAMIC_CAST 0
#define VCL_STATIC_CONST_INIT_FLOAT(x) = x
#define VCL_STATIC_CONST_INIT_INT(x) = x
#define VCL_IMPLEMENT_STATIC_CONSTS 0
#define VCL_INSTANTIATE_INLINE(fn_decl) template fn_decl ;
#define VCL_DO_NOT_INSTANTIATE(text,ret) text {return ret;}
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x) extern x;
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 1 //(text) friend text;
#define VCL_OVERLOAD_CAST(T,x) ((T)(x))
#define VCL_DECLARE_SPECIALIZATION(f) /* template <> f; */
#define VCL_DEFINE_SPECIALIZATION /* template <> */
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#define VCL_DEFAULT_VALUE(x) =x
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// egcs
#if defined(VCL_EGCS)
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_DYNAMIC_CAST 0 // as long as we use -fno-rtti ...
#define VCL_HAS_MEMBER_TEMPLATES 1
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 1
#define VCL_STATIC_CONST_INIT_FLOAT(x) = x
#define VCL_STATIC_CONST_INIT_INT(x) = x
#define VCL_IMPLEMENT_STATIC_CONSTS 0
#define VCL_INSTANTIATE_INLINE(fn_decl) 
#define VCL_DO_NOT_INSTANTIATE(text,ret) text {return ret;}
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 0 //(text)
#define VCL_OVERLOAD_CAST(T,x) /*(T)*/(x)
#define VCL_DECLARE_SPECIALIZATION(f) template <> f;
#define VCL_DEFINE_SPECIALIZATION template <>
#define VCL_STL_NULL_TMPL_ARGS <>
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// gcc 2.8.x
#if defined(VCL_GCC_28)
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_DYNAMIC_CAST 0 // as long as we use -fno-rtti ...
#define VCL_HAS_MEMBER_TEMPLATES 1
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 1
#define VCL_STATIC_CONST_INIT_FLOAT(x) = x
#define VCL_STATIC_CONST_INIT_INT(x) = x
#define VCL_IMPLEMENT_STATIC_CONSTS 0
#define VCL_INSTANTIATE_INLINE(fn_decl) 
#define VCL_DO_NOT_INSTANTIATE(text,ret) text {return ret;}
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 0 //(text)
#define VCL_OVERLOAD_CAST(T,x) /*(T)*/(x)
#define VCL_DECLARE_SPECIALIZATION(f) template <> f;
#define VCL_DEFINE_SPECIALIZATION template <>
#define VCL_STL_NULL_TMPL_ARGS <>
#define VCL_DEFAULT_VALUE(x) =x
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// gcc 2.95
#if defined(VCL_GCC_295)
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_DYNAMIC_CAST 0 // as long as we use -fno-rtti ...
#define VCL_HAS_MEMBER_TEMPLATES 1
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 1
#define VCL_STATIC_CONST_INIT_FLOAT(x) = x
#define VCL_STATIC_CONST_INIT_INT(x) = x
#define VCL_IMPLEMENT_STATIC_CONSTS 0
#define VCL_INSTANTIATE_INLINE(fn_decl) 
#define VCL_DO_NOT_INSTANTIATE(text,ret) text {return ret;}
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 0 //(text)
#define VCL_OVERLOAD_CAST(T,x) /*(T)*/(x)
#define VCL_DECLARE_SPECIALIZATION(f) template <> f;
#define VCL_DEFINE_SPECIALIZATION template <>
#if 0
// Rather splendidly, gcc 2.95.1 bails if these are correctly specified...
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#else
// But 2.95.2 needs them, and that compiler is cleverer that 2.95.1, so:
#define VCL_STL_NULL_TMPL_ARGS <>
#endif
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// windows (vc50)
#ifdef VCL_WIN32
#define VCL_FOR_SCOPE_HACK 1
#define VCL_HAS_DYNAMIC_CAST 1
#ifdef VCL_VC50
# define VCL_HAS_MEMBER_TEMPLATES 0
#else
# define VCL_HAS_MEMBER_TEMPLATES 1
#endif
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0
#define VCL_STATIC_CONST_INIT_FLOAT(x)
#define VCL_STATIC_CONST_INIT_INT(x)
#define VCL_IMPLEMENT_STATIC_CONSTS 1
#define VCL_INSTANTIATE_INLINE(fn_decl) template fn_decl ;
#define VCL_DO_NOT_INSTANTIATE(text,ret) text {return ret;}
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 0 //(text)
#define VCL_OVERLOAD_CAST(T,x) /*(T)*/(x)
#define VCL_DECLARE_SPECIALIZATION(f) /* template <> f; */
#define VCL_DEFINE_SPECIALIZATION /* template <> */
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A
// vc60(function.h) needs ,classname here: 
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// native sgi < 7
#if defined(VCL_SGI_CC_6)
#define VCL_FOR_SCOPE_HACK 1
#define VCL_HAS_DYNAMIC_CAST 0
#define VCL_HAS_MEMBER_TEMPLATES 0
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0
#define VCL_IMPLEMENT_STATIC_CONSTS 1
#define VCL_STATIC_CONST_INIT_FLOAT(x)
#define VCL_STATIC_CONST_INIT_INT(x)
#define VCL_INSTANTIATE_INLINE(fn_decl)
#define VCL_DO_NOT_INSTANTIATE(text,ret) @pragma do_not_instantiate text@
#define VCL_UNINSTANTIATE_SPECIALIZATION(text) @pragma do_not_instantiate text@
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x) @pragma do_not_instantiate x@
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x) @pragma do_not_instantiate x@
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 1 //(text) friend text;
#define VCL_OVERLOAD_CAST(T,x) ((T)(x))
#define __SGI_STL_NO_ARROW_OPERATOR
#define VCL_DECLARE_SPECIALIZATION(f) /* template <> f; */
#define VCL_DEFINE_SPECIALIZATION /* template <> */
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// native sgi 7
#ifdef VCL_SGI_CC_7
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_DYNAMIC_CAST 1 // now it is -- fsm. 0 // should be 1
#define VCL_HAS_MEMBER_TEMPLATES 0
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0
#define VCL_IMPLEMENT_STATIC_CONSTS 1
#define VCL_STATIC_CONST_INIT_FLOAT(x)
#define VCL_STATIC_CONST_INIT_INT(x)
#define VCL_INSTANTIATE_INLINE(fn_decl)
#define VCL_DO_NOT_INSTANTIATE(text,ret) text { return ret; }
#define VCL_UNINSTANTIATE_SPECIALIZATION(text)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x) @pragma do_not_instantiate x@
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x) @pragma do_not_instantiate x@
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x;
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 1 //(text) friend text;
#define VCL_OVERLOAD_CAST(T,x) ((T)(x))
#define __SGI_STL_NO_ARROW_OPERATOR
#define VCL_DECLARE_SPECIALIZATION(f) /* template <> f; */
#define VCL_DEFINE_SPECIALIZATION /* template <> */
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A = a
#define VCL_DFL_TMPL_ARG(classname) , classname
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T
#endif

// SunPro < 5 (eg 4.2)
#if defined(VCL_SUNPRO_CC) && !defined(VCL_SUNPRO_CC_50)
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_DYNAMIC_CAST 0
#define VCL_HAS_MEMBER_TEMPLATES 0
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0
#define VCL_STATIC_CONST_INIT_FLOAT(x)
#define VCL_STATIC_CONST_INIT_INT(x)
#define VCL_IMPLEMENT_STATIC_CONSTS 1
#define VCL_INSTANTIATE_INLINE(fn_decl) 
#define VCL_DO_NOT_INSTANTIATE(text,ret) text {return ret;}
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x; 
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 0 //(text)
#define VCL_OVERLOAD_CAST(T,x) /*(T)*/(x)
#define VCL_DECLARE_SPECIALIZATION(f) /* template <> f; */
#define VCL_DEFINE_SPECIALIZATION /* template <> */
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A
#define VCL_DFL_TMPL_ARG(classname) , classname
// The sunpro is ok with static data as long as it is only seen
// at instantiation time, like in stl
#define VCL_NO_STATIC_DATA_MEMBERS 1
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T , allocator<T >
#endif

// SunPro 5.0
#if defined(VCL_SUNPRO_CC_50)
#define VCL_FOR_SCOPE_HACK 0
#define VCL_HAS_DYNAMIC_CAST 0
#define VCL_HAS_MEMBER_TEMPLATES 0
#define VCL_CAN_DO_PARTIAL_SPECIALIZATION 0
#define VCL_STATIC_CONST_INIT_FLOAT(x)
#define VCL_STATIC_CONST_INIT_INT(x)
#define VCL_IMPLEMENT_STATIC_CONSTS 1
#define VCL_INSTANTIATE_INLINE(fn_decl) 
#define VCL_DO_NOT_INSTANTIATE(text,ret) template <> text {return ret;}
#define VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(x) x; 
#define VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(x)
#define VCL_UNINSTANTIATE_SPECIALIZATION(x)
#define VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(x)
#define VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD 0 //(text)
#define VCL_OVERLOAD_CAST(T,x) /*(T)*/(x)
#define VCL_DECLARE_SPECIALIZATION(f) template <> f;
#define VCL_DEFINE_SPECIALIZATION template <>
#define VCL_STL_NULL_TMPL_ARGS /* <> */
#define VCL_DEFAULT_VALUE(x)
#define VCL_DFL_TYPE_PARAM_STLDECL(A,a) class A
#define VCL_DFL_TMPL_PARAM_STLDECL(A,a) class A
#define VCL_DFL_TMPL_ARG(classname) , classname
// The sunpro is ok with static data as long as it is only seen
// at instantiation time, like in stl
#define VCL_NO_STATIC_DATA_MEMBERS 1
#define VCL_SUNPRO_ALLOCATOR_HACK(T) T , std :: allocator<T >
#endif

//--------------------------------------------------------------------------------

// Decide at configuration time whether you want to use long double.  On most
// machines it's too slow.
#if defined(VCL_USE_LONG_DOUBLE)
typedef long double VCL_long_double;
#else
typedef double VCL_long_double;
#endif

#if VCL_FOR_SCOPE_HACK
# undef for
# define for if (0) {} else for
typedef int saw_VCL_FOR_SCOPE_HACK;
#endif

// fix to instantiate template functions
#define VCL_INSTANTIATE_NONINLINE(fn_decl) template fn_decl ;

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF VCL_compiler.
