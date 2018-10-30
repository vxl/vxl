#ifndef vcl_compiler_h_
#define vcl_compiler_h_
#define VXL_LEGACY_FUTURE_REMOVE
//:
// \file
// \brief Uniform macro definition scheme for finding out about the compiler
//
// It's much better to determine the compiler automatically here than to depend
// on command-line flags being set.
//
// Note that this is most commonly implemented using a cascade of if
// statements. Be careful to add your statements to the correct place
// in the cascade list.
//
// Naming scheme:
// If you have a compiler name XYZ, then
//     #define VCL_XYZ
// Each each major release, define a release number
//     #define VCL_XYZ_4
// Avoid using the marketing name for the release number, because it's
// harder to follow. For example, Microsoft Visual C++ .NET 2003 is
// better called Visual C++ 7.
// For each minor version, define the appropriate minor version number
//     #define VCL_XYZ_40
// If necessary, define the patchlevel too:
//     #define VCL_XYZ_401
//
// Make sure that if the minor version is defined, then the release
// number and the compiler name are also defined.
//
// Add the corresponding test to tests/test_platform to make sure.

#if defined(__GNUC__) && !defined(__ICC) // icc 8.0 defines __GNUC__
# define VCL_GCC
# if (__GNUC__ < 4)
#  error "Invalid VCL_GCC version not supported."
# endif
#define VCL_GCC_4  "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_40 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_41 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_42 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_43 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_5  "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_50 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_51 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_52 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_53 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_6  "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_60 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_61 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_62 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_63 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_7  "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_70  "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_71 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_GCC_72 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#endif

#if defined(_WIN32) || defined(WIN32)
# define VCL_WIN32

# if defined(_MSC_VER)
#  define VCL_VC
#  if _MSC_VER < 1900     // Visual Studio 2015 = Version 14.x is needed to support
#   error "Invalid VCL_VC version Visual Studio 2015 = Version 14.x is needed to support C++11 features"
#  endif // <1900
// Disable some common warnings in MS VC++. This is the set of warnings suppressed by ITK
#  pragma warning ( disable : 4244 ) // 'conversion' conversion from 'type1' to 'type2', possible loss of data
#  pragma warning ( disable : 4305 ) // 'identifier' : truncation from 'type1' to 'type2'
#  pragma warning ( disable : 4309 ) // 'conversion' : truncation of constant value
#  pragma warning ( disable : 4503 ) // decorated name length exceeded, name was truncated
#  pragma warning ( disable : 4800 ) // 'type' : forcing value to bool 'true' or 'false' (performance warning)
#  pragma warning ( disable : 4251 ) // 'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
#  pragma warning ( disable : 4275 ) // non dll-interface class 'type' used as base for dll-interface class 'type2'
#  pragma warning ( disable : 4290 ) // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#  pragma warning ( disable : 4273 ) // 'type' : inconsistent dll linkage.  dllexport assumed.
#  pragma warning ( disable : 4127 ) // conditional expression is constant
#  pragma warning ( disable : 4505 ) // unreferenced local function has been removed
#  pragma warning ( disable : 4786 ) // 'identifier' : identifier was truncated to 'number' characters in the debug information
#  pragma warning ( disable : 4231 ) // nonstandard extension used : 'extern' before template explicit instantiation
#  pragma warning ( disable : 4267 ) // data-conversion related to 'size_t'
# endif // _MSC_VER

#define VCL_VC_DOTNET "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_VC_9 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_VC_10 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#define VCL_VC_11 "VXL no longer supplies aliases for compiler versions, use the compiler defines directly"
#endif // WIN32


// include header files generated by configure.
#include <vcl_config_compiler.h>

// -------------------- default template parameters
#if VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER
# define VCL_DFL_TYPE_PARAM_STLDECL(A, a) class A = a
#else
# define VCL_DFL_TYPE_PARAM_STLDECL(A, a) class A
#endif

#if VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER
# define VCL_DFL_TMPL_PARAM_STLDECL(A, a) class A = a
#else
# define VCL_DFL_TMPL_PARAM_STLDECL(A, a) class A
#endif

#define VCL_DFL_TMPL_ARG(classname) , classname

// -------------------- handy macros ---------------------------------------

//: VCL_COMMA
//
// Handy for passing things with commas in them to CPP macros.  e.g.
// DO_MACRO(pair<A,B>) can be replaced by DO_MACRO(pair<A VCL_COMMA B>).
#define VCL_COMMA ,

//----------------------------------------------------------------------------
#if defined(NDEBUG)
# define VXL_USED_IN_DEBUG(x)
/* Valgrind does not support "long double" types and this results
 * in numerous false positive memory errors being reported.
 * Turn off tests involving "long double" types by default
 * when compiling in "Debug" mode.
 * You can override this behavior by including a compiler
 * flag of -DINCLUDE_LONG_DOUBLE_TESTS when compiling
 * in debug mode to enable these tests.
 */
# define INCLUDE_LONG_DOUBLE_TESTS
#else
# define VXL_USED_IN_DEBUG(x) x
#endif

//----------------------------------------------------------------------------
// Macros for safe-bool idiom.
# define VCL_SAFE_BOOL_TRUE (&safe_bool_dummy::dummy)
# define VCL_SAFE_BOOL_DEFINE \
   struct safe_bool_dummy { void dummy() {} }; \
   typedef void (safe_bool_dummy::* safe_bool)()

//----------------------------------------------------------------------
// constant initializer issues.
#ifdef VXL_LEGACY_FUTURE_REMOVE
# define VXL_CONSTEXPR_FUNC constexpr  "ERROR: VXL_CONSTEXPR_FUNC deprecated, use C++11 constexpr"
# define VXL_CONSTEXPR_VAR  constexpr  "ERROR: VXL_CONSTEXPR_VAR deprecated, use C++11 constexpr"
# define VXL_FULLCXX11SUPPORT "ERROR: ONLY VXL_FULLCXX11SUPPORT supported"
# define VCL_STATIC_CONST_INIT_INT_DECL(x) "ERROR: repace VCL_STATIC_CONST_INIT_INT_DECL with ' = '"
# define VCL_STATIC_CONST_INIT_INT_DEFN(x) "ERROR: VCL_STATIC_CONST_INIT_INT_DEFN is a noop"
# define VCL_STATIC_CONST_INIT_FLOAT_DECL(x) "ERROR: repace VCL_STATIC_CONST_INIT_FLOAT_DECL with ' = '"
# define VCL_STATIC_CONST_INIT_FLOAT_DEFN(x) "ERROR: VCL_STATIC_CONST_INIT_FLOAT_DEFN is a noop"
//# define VCL_SAFE_BOOL_TRUE "ERROR: Use true for VCL_SAFE_BOOL_TRUE"
//# define VCL_SAFE_BOOL_DEFINE "ERROR: VCL_SAFE_BOOL_DEFINE is a noop in C++11"

/* Check for C99 versions of is[finite|inf|nan|normal] in <cmath> */
#define VXL_HAS_STD_ISFINITE    "ERROR:  Use C++11 cmath"
#define VXL_HAS_STD_ISINF    "ERROR:  Use C++11 cmath"
#define VXL_HAS_STD_ISNAN    "ERROR:  Use C++11 cmath"
#define VXL_HAS_STD_ISNORMAL "ERROR:  Use C++11 cmath"
#define VXL_IEEEFP_HAS_FINITE   "ERROR:  Use C++11 cmath"
#define VXL_C_MATH_HAS_FINITEF  "ERROR:  Use C++11 cmath"
#define VXL_C_MATH_HAS_FINITE   "ERROR:  Use C++11 cmath"
#define VXL_C_MATH_HAS_FINITEL  "ERROR:  Use C++11 cmath"
#define VXL_C_MATH_HAS_SQRTF    "ERROR:  Use C++11 cmath"
#define VXL_STDLIB_HAS_QSORT   "ERROR:  Use C++11 cstdlib"

#define VXL_STDLIB_HAS_LRAND48 "ERROR:  Use C++11 cstdlib"
#define VXL_STDLIB_HAS_DRAND48 "ERROR:  Use C++11 cstdlib"
#define VXL_STDLIB_HAS_SRAND48 "ERROR:  Use C++11 cstdlib"

#define VCL_FREEBSD "ERROR: VCL_FREBSD has no use in VXL"
#define VCL_VOID_RETURN "ERROR: VCL_VOID_RETURN has no use in VXL"
#define VCL_CYGWIN_GCC "ERROR: VCL_CYGWIN_GCC has no use in VXL"

//REDUNDANT WITH write_compiler_detection_header
#define VCL_HAS_EXPORT "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_BOOL "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_TYPENAME "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_MUTABLE "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_DYNAMIC_CAST "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_RTTI "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_DEFAULT_VALUE "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_MEMBER_TEMPLATES "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_CAN_DO_STATIC_TEMPLATE_MEMBER "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_EXCEPTIONS "ERROR: Removed in favor of write_compiler_detection_header detections"
#define VCL_HAS_NAMESPACE "ERROR: Removed in favor of write_compiler_detection_header detections"

#define VCL_INCLUDE_CXX_0X "ERROR: VCL_INCLUDE_CXX_0X is always 0 in C++11”
#define VCL_MEMORY_HAS_SHARED_PTR "ERRROR: VCL_MEMORY_HAS_SHARED_PTR is always 1 in C++11”
#define VCL_MEMORY_HAS_TR1_SHARED_PTR "ERROR: VCL_MEMORY_HAS_TR1_SHARED_PTR is always 0 in C++11”
#define VCL_TR1_MEMORY_HAS_SHARED_PTR "ERROR: VCL_TR1_MEMORY_HAS_SHARED_PTR is always 0 in C++11”
#define VCL_USE_ATOMIC_COUNT "ERROR: VCL_USE_ATOMIC_COUNT is always defined in C++11”
#define VCL_CXX_HAS_TR1 "ERROR: C++11 never has TR1”


#endif

#ifdef VNL_CONFIG_LEGACY_METHODS
#include "vcl_legacy_aliases.h"
#endif

#endif // vcl_compiler_h_
