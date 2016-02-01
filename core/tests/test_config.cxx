// Do not edit this file.  It is created automatically from make_test_config.pl

#include <vcl_compiler.h>
#include <vxl_config.h>
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>

// It is quite difficult to do anything out of the norm with the C preprocessor

// quote() has the effect of preprocessor expansion of the argument
// Inner parentheses are needed with quote() to protect empty definitions
// and definitions that have commas
// quote0() protects (") and quotes ("") the expanded argument
#define quote0(x) #x
#define quote(x) quote0((x))

void test_config()
{
  // This test does not test much.  Mostly, it prints configuration
  // macros so they can be viewed through Dart.  Some sanity checks are
  // at the end.

  vcl_cout << "All macro definitions are shown inside parentheses\n";

  vcl_cout << "VCL_HAS_BOOL ";
#ifdef VCL_HAS_BOOL
  vcl_cout << "is set to " << quote(VCL_HAS_BOOL);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_DYNAMIC_CAST ";
#ifdef VCL_HAS_DYNAMIC_CAST
  vcl_cout << "is set to " << quote(VCL_HAS_DYNAMIC_CAST);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_RTTI ";
#ifdef VCL_HAS_RTTI
  vcl_cout << "is set to " << quote(VCL_HAS_RTTI);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_TYPENAME ";
#ifdef VCL_HAS_TYPENAME
  vcl_cout << "is set to " << quote(VCL_HAS_TYPENAME);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_EXPORT ";
#ifdef VCL_HAS_EXPORT
  vcl_cout << "is set to " << quote(VCL_HAS_EXPORT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_MUTABLE ";
#ifdef VCL_HAS_MUTABLE
  vcl_cout << "is set to " << quote(VCL_HAS_MUTABLE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_EXPLICIT ";
#ifdef VCL_HAS_EXPLICIT
  vcl_cout << "is set to " << quote(VCL_HAS_EXPLICIT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_FOR_SCOPE_HACK ";
#ifdef VCL_FOR_SCOPE_HACK
  vcl_cout << "is set to " << quote(VCL_FOR_SCOPE_HACK);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_DEFAULT_VALUE(x) ";
#ifdef VCL_DEFAULT_VALUE
  vcl_cout << "is set to " << quote(VCL_DEFAULT_VALUE(x));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_STATIC_CONST_INIT_INT ";
#ifdef VCL_CAN_STATIC_CONST_INIT_INT
  vcl_cout << "is set to " << quote(VCL_CAN_STATIC_CONST_INIT_INT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_STATIC_CONST_INIT_INT_DECL(x) ";
#ifdef VCL_STATIC_CONST_INIT_INT_DECL
  vcl_cout << "is set to " << quote(VCL_STATIC_CONST_INIT_INT_DECL(x));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_STATIC_CONST_INIT_INT_DEFN(x) ";
#ifdef VCL_STATIC_CONST_INIT_INT_DEFN
  vcl_cout << "is set to " << quote(VCL_STATIC_CONST_INIT_INT_DEFN(x));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_STATIC_CONST_INIT_INT_NO_DEFN ";
#ifdef VCL_STATIC_CONST_INIT_INT_NO_DEFN
  vcl_cout << "is set to " << quote(VCL_STATIC_CONST_INIT_INT_NO_DEFN);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_STATIC_CONST_INIT_FLOAT ";
#ifdef VCL_CAN_STATIC_CONST_INIT_FLOAT
  vcl_cout << "is set to " << quote(VCL_CAN_STATIC_CONST_INIT_FLOAT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_STATIC_CONST_INIT_FLOAT_DECL(x) ";
#ifdef VCL_STATIC_CONST_INIT_FLOAT_DECL
  vcl_cout << "is set to " << quote(VCL_STATIC_CONST_INIT_FLOAT_DECL(x));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_STATIC_CONST_INIT_FLOAT_DEFN(x) ";
#ifdef VCL_STATIC_CONST_INIT_FLOAT_DEFN
  vcl_cout << "is set to " << quote(VCL_STATIC_CONST_INIT_FLOAT_DEFN(x));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_STATIC_CONST_INIT_FLOAT_NO_DEFN ";
#ifdef VCL_STATIC_CONST_INIT_FLOAT_NO_DEFN
  vcl_cout << "is set to " << quote(VCL_STATIC_CONST_INIT_FLOAT_NO_DEFN);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_MEMBER_TEMPLATES ";
#ifdef VCL_HAS_MEMBER_TEMPLATES
  vcl_cout << "is set to " << quote(VCL_HAS_MEMBER_TEMPLATES);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_DO_PARTIAL_SPECIALIZATION ";
#ifdef VCL_CAN_DO_PARTIAL_SPECIALIZATION
  vcl_cout << "is set to " << quote(VCL_CAN_DO_PARTIAL_SPECIALIZATION);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_DEFINE_SPECIALIZATION ";
#ifdef VCL_DEFINE_SPECIALIZATION
  vcl_cout << "is set to " << quote(VCL_DEFINE_SPECIALIZATION);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CANNOT_SPECIALIZE_CV ";
#ifdef VCL_CANNOT_SPECIALIZE_CV
  vcl_cout << "is set to " << quote(VCL_CANNOT_SPECIALIZE_CV);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_NULL_TMPL_ARGS ";
#ifdef VCL_NULL_TMPL_ARGS
  vcl_cout << "is set to " << quote(VCL_NULL_TMPL_ARGS);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_ALLOWS_INLINE_INSTANTIATION ";
#ifdef VCL_ALLOWS_INLINE_INSTANTIATION
  vcl_cout << "is set to " << quote(VCL_ALLOWS_INLINE_INSTANTIATION);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_NEEDS_INLINE_INSTANTIATION ";
#ifdef VCL_NEEDS_INLINE_INSTANTIATION
  vcl_cout << "is set to " << quote(VCL_NEEDS_INLINE_INSTANTIATION);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_DO_NOT_INSTANTIATE(text,ret) ";
#ifdef VCL_DO_NOT_INSTANTIATE
  vcl_cout << "is set to " << quote(VCL_DO_NOT_INSTANTIATE(text,ret));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_UNINSTANTIATE_SPECIALIZATION(symbol) ";
#ifdef VCL_UNINSTANTIATE_SPECIALIZATION
  vcl_cout << "is set to " << quote(VCL_UNINSTANTIATE_SPECIALIZATION(symbol));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(symbol) ";
#ifdef VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION
  vcl_cout << "is set to " << quote(VCL_UNINSTANTIATE_UNSEEN_SPECIALIZATION(symbol));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_DO_STATIC_TEMPLATE_MEMBER ";
#ifdef VCL_CAN_DO_STATIC_TEMPLATE_MEMBER
  vcl_cout << "is set to " << quote(VCL_CAN_DO_STATIC_TEMPLATE_MEMBER);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(symbol) ";
#ifdef VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER
  vcl_cout << "is set to " << quote(VCL_INSTANTIATE_STATIC_TEMPLATE_MEMBER(symbol));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(symbol) ";
#ifdef VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER
  vcl_cout << "is set to " << quote(VCL_UNINSTANTIATE_STATIC_TEMPLATE_MEMBER(symbol));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER ";
#ifdef VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER
  vcl_cout << "is set to " << quote(VCL_CAN_DO_NON_TYPE_FUNCTION_TEMPLATE_PARAMETER);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD ";
#ifdef VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD
  vcl_cout << "is set to " << quote(VCL_NEED_FRIEND_FOR_TEMPLATE_OVERLOAD);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_OVERLOAD_CAST(T,x) ";
#ifdef VCL_OVERLOAD_CAST
  vcl_cout << "is set to " << quote(VCL_OVERLOAD_CAST(T,x));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_NO_STATIC_DATA_MEMBERS ";
#ifdef VCL_NO_STATIC_DATA_MEMBERS
  vcl_cout << "is set to " << quote(VCL_NO_STATIC_DATA_MEMBERS);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_TEMPLATE_SYMBOLS ";
#ifdef VCL_HAS_TEMPLATE_SYMBOLS
  vcl_cout << "is set to " << quote(VCL_HAS_TEMPLATE_SYMBOLS);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_DEFAULT_TMPL_ARG(arg) ";
#ifdef VCL_DEFAULT_TMPL_ARG
  vcl_cout << "is set to " << quote(VCL_DEFAULT_TMPL_ARG(arg));
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER ";
#ifdef VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER
  vcl_cout << "is set to " << quote(VCL_CAN_DO_COMPLETE_DEFAULT_TYPE_PARAMETER);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER ";
#ifdef VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER
  vcl_cout << "is set to " << quote(VCL_CAN_DO_TEMPLATE_DEFAULT_TYPE_PARAMETER);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_EXCEPTIONS ";
#ifdef VCL_HAS_EXCEPTIONS
  vcl_cout << "is set to " << quote(VCL_HAS_EXCEPTIONS);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_HAS_NAMESPACES ";
#ifdef VCL_HAS_NAMESPACES
  vcl_cout << "is set to " << quote(VCL_HAS_NAMESPACES);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_ALLOWS_NAMESPACE_STD ";
#ifdef VCL_ALLOWS_NAMESPACE_STD
  vcl_cout << "is set to " << quote(VCL_ALLOWS_NAMESPACE_STD);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_NEEDS_NAMESPACE_STD ";
#ifdef VCL_NEEDS_NAMESPACE_STD
  vcl_cout << "is set to " << quote(VCL_NEEDS_NAMESPACE_STD);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CASSERT ";
#ifdef VCL_CXX_HAS_HEADER_CASSERT
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CASSERT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CSETJMP ";
#ifdef VCL_CXX_HAS_HEADER_CSETJMP
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CSETJMP);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CSTDIO ";
#ifdef VCL_CXX_HAS_HEADER_CSTDIO
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CSTDIO);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CTIME ";
#ifdef VCL_CXX_HAS_HEADER_CTIME
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CTIME);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CCTYPE ";
#ifdef VCL_CXX_HAS_HEADER_CCTYPE
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CCTYPE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CLIMITS ";
#ifdef VCL_CXX_HAS_HEADER_CLIMITS
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CLIMITS);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CSIGNAL ";
#ifdef VCL_CXX_HAS_HEADER_CSIGNAL
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CSIGNAL);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CWCHAR ";
#ifdef VCL_CXX_HAS_HEADER_CWCHAR
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CWCHAR);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CERRNO ";
#ifdef VCL_CXX_HAS_HEADER_CERRNO
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CERRNO);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CLOCALE ";
#ifdef VCL_CXX_HAS_HEADER_CLOCALE
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CLOCALE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CSTDARG ";
#ifdef VCL_CXX_HAS_HEADER_CSTDARG
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CSTDARG);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CSTRING ";
#ifdef VCL_CXX_HAS_HEADER_CSTRING
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CSTRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CWCTYPE ";
#ifdef VCL_CXX_HAS_HEADER_CWCTYPE
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CWCTYPE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CMATH ";
#ifdef VCL_CXX_HAS_HEADER_CMATH
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CMATH);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_CSTDDEF ";
#ifdef VCL_CXX_HAS_HEADER_CSTDDEF
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_CSTDDEF);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_ALGORITHM ";
#ifdef VCL_CXX_HAS_HEADER_ALGORITHM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_ALGORITHM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_IOMANIP ";
#ifdef VCL_CXX_HAS_HEADER_IOMANIP
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_IOMANIP);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_LIST ";
#ifdef VCL_CXX_HAS_HEADER_LIST
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_LIST);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_OSTREAM ";
#ifdef VCL_CXX_HAS_HEADER_OSTREAM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_OSTREAM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_STREAMBUF ";
#ifdef VCL_CXX_HAS_HEADER_STREAMBUF
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_STREAMBUF);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_BITSET ";
#ifdef VCL_CXX_HAS_HEADER_BITSET
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_BITSET);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_IOS ";
#ifdef VCL_CXX_HAS_HEADER_IOS
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_IOS);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_LOCALE ";
#ifdef VCL_CXX_HAS_HEADER_LOCALE
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_LOCALE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_QUEUE ";
#ifdef VCL_CXX_HAS_HEADER_QUEUE
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_QUEUE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_STRING ";
#ifdef VCL_CXX_HAS_HEADER_STRING
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_COMPLEX ";
#ifdef VCL_CXX_HAS_HEADER_COMPLEX
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_COMPLEX);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_IOSFWD ";
#ifdef VCL_CXX_HAS_HEADER_IOSFWD
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_IOSFWD);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_MAP ";
#ifdef VCL_CXX_HAS_HEADER_MAP
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_MAP);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_SET ";
#ifdef VCL_CXX_HAS_HEADER_SET
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_SET);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_TYPEINFO ";
#ifdef VCL_CXX_HAS_HEADER_TYPEINFO
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_TYPEINFO);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_DEQUE ";
#ifdef VCL_CXX_HAS_HEADER_DEQUE
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_DEQUE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_IOSTREAM ";
#ifdef VCL_CXX_HAS_HEADER_IOSTREAM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_IOSTREAM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_MEMORY ";
#ifdef VCL_CXX_HAS_HEADER_MEMORY
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_MEMORY);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_SSTREAM ";
#ifdef VCL_CXX_HAS_HEADER_SSTREAM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_SSTREAM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_UTILITY ";
#ifdef VCL_CXX_HAS_HEADER_UTILITY
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_UTILITY);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_EXCEPTION ";
#ifdef VCL_CXX_HAS_HEADER_EXCEPTION
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_EXCEPTION);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_ISTREAM ";
#ifdef VCL_CXX_HAS_HEADER_ISTREAM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_ISTREAM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_NEW ";
#ifdef VCL_CXX_HAS_HEADER_NEW
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_NEW);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_STACK ";
#ifdef VCL_CXX_HAS_HEADER_STACK
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_STACK);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_VALARRAY ";
#ifdef VCL_CXX_HAS_HEADER_VALARRAY
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_VALARRAY);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_FSTREAM ";
#ifdef VCL_CXX_HAS_HEADER_FSTREAM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_FSTREAM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_ITERATOR ";
#ifdef VCL_CXX_HAS_HEADER_ITERATOR
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_ITERATOR);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_NUMERIC ";
#ifdef VCL_CXX_HAS_HEADER_NUMERIC
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_NUMERIC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_STDEXCEPT ";
#ifdef VCL_CXX_HAS_HEADER_STDEXCEPT
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_STDEXCEPT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_VECTOR ";
#ifdef VCL_CXX_HAS_HEADER_VECTOR
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_VECTOR);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_FUNCTIONAL ";
#ifdef VCL_CXX_HAS_HEADER_FUNCTIONAL
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_FUNCTIONAL);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_CXX_HAS_HEADER_STRSTREAM ";
#ifdef VCL_CXX_HAS_HEADER_STRSTREAM
  vcl_cout << "is set to " << quote(VCL_CXX_HAS_HEADER_STRSTREAM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_LITTLE_ENDIAN ";
#ifdef VXL_LITTLE_ENDIAN
  vcl_cout << "is set to " << quote(VXL_LITTLE_ENDIAN);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_BIG_ENDIAN ";
#ifdef VXL_BIG_ENDIAN
  vcl_cout << "is set to " << quote(VXL_BIG_ENDIAN);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_BYTE ";
#ifdef VXL_HAS_BYTE
  vcl_cout << "is set to " << quote(VXL_HAS_BYTE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_BYTE_STRING ";
#ifdef VXL_BYTE_STRING
  vcl_cout << "is set to " << quote(VXL_BYTE_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_INT_8 ";
#ifdef VXL_HAS_INT_8
  vcl_cout << "is set to " << quote(VXL_HAS_INT_8);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_INT_8_STRING ";
#ifdef VXL_INT_8_STRING
  vcl_cout << "is set to " << quote(VXL_INT_8_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_INT_16 ";
#ifdef VXL_HAS_INT_16
  vcl_cout << "is set to " << quote(VXL_HAS_INT_16);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_INT_16_STRING ";
#ifdef VXL_INT_16_STRING
  vcl_cout << "is set to " << quote(VXL_INT_16_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_INT_32 ";
#ifdef VXL_HAS_INT_32
  vcl_cout << "is set to " << quote(VXL_HAS_INT_32);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_INT_32_STRING ";
#ifdef VXL_INT_32_STRING
  vcl_cout << "is set to " << quote(VXL_INT_32_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_INT_64 ";
#ifdef VXL_HAS_INT_64
  vcl_cout << "is set to " << quote(VXL_HAS_INT_64);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_INT_64_STRING ";
#ifdef VXL_INT_64_STRING
  vcl_cout << "is set to " << quote(VXL_INT_64_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_INT_64_IS_LONG ";
#ifdef VXL_INT_64_IS_LONG
  vcl_cout << "is set to " << quote(VXL_INT_64_IS_LONG);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_IEEE_32 ";
#ifdef VXL_HAS_IEEE_32
  vcl_cout << "is set to " << quote(VXL_HAS_IEEE_32);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_IEEE_32_STRING ";
#ifdef VXL_IEEE_32_STRING
  vcl_cout << "is set to " << quote(VXL_IEEE_32_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_IEEE_64 ";
#ifdef VXL_HAS_IEEE_64
  vcl_cout << "is set to " << quote(VXL_HAS_IEEE_64);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_IEEE_64_STRING ";
#ifdef VXL_IEEE_64_STRING
  vcl_cout << "is set to " << quote(VXL_IEEE_64_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_IEEE_96 ";
#ifdef VXL_HAS_IEEE_96
  vcl_cout << "is set to " << quote(VXL_HAS_IEEE_96);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_IEEE_96_STRING ";
#ifdef VXL_IEEE_96_STRING
  vcl_cout << "is set to " << quote(VXL_IEEE_96_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_IEEE_128 ";
#ifdef VXL_HAS_IEEE_128
  vcl_cout << "is set to " << quote(VXL_HAS_IEEE_128);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_IEEE_128_STRING ";
#ifdef VXL_IEEE_128_STRING
  vcl_cout << "is set to " << quote(VXL_IEEE_128_STRING);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_PTHREAD_H ";
#ifdef VXL_HAS_PTHREAD_H
  vcl_cout << "is set to " << quote(VXL_HAS_PTHREAD_H);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_SEMAPHORE_H ";
#ifdef VXL_HAS_SEMAPHORE_H
  vcl_cout << "is set to " << quote(VXL_HAS_SEMAPHORE_H);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_USECONDS_T ";
#ifdef VXL_UNISTD_HAS_USECONDS_T
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_USECONDS_T);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_INTPTR_T ";
#ifdef VXL_UNISTD_HAS_INTPTR_T
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_INTPTR_T);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_UALARM ";
#ifdef VXL_UNISTD_HAS_UALARM
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_UALARM);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_USLEEP ";
#ifdef VXL_UNISTD_HAS_USLEEP
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_USLEEP);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_LCHOWN ";
#ifdef VXL_UNISTD_HAS_LCHOWN
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_LCHOWN);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_PREAD ";
#ifdef VXL_UNISTD_HAS_PREAD
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_PREAD);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_PWRITE ";
#ifdef VXL_UNISTD_HAS_PWRITE
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_PWRITE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_TELL ";
#ifdef VXL_UNISTD_HAS_TELL
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_TELL);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_GETPID ";
#ifdef VXL_UNISTD_HAS_GETPID
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_GETPID);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_HAS_GETHOSTNAME ";
#ifdef VXL_UNISTD_HAS_GETHOSTNAME
  vcl_cout << "is set to " << quote(VXL_UNISTD_HAS_GETHOSTNAME);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_STDLIB_HAS_QSORT ";
#ifdef VXL_STDLIB_HAS_QSORT
  vcl_cout << "is set to " << quote(VXL_STDLIB_HAS_QSORT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_STDLIB_HAS_LRAND48 ";
#ifdef VXL_STDLIB_HAS_LRAND48
  vcl_cout << "is set to " << quote(VXL_STDLIB_HAS_LRAND48);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_STDLIB_HAS_DRAND48 ";
#ifdef VXL_STDLIB_HAS_DRAND48
  vcl_cout << "is set to " << quote(VXL_STDLIB_HAS_DRAND48);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_IEEEFP_HAS_FINITE ";
#ifdef VXL_IEEEFP_HAS_FINITE
  vcl_cout << "is set to " << quote(VXL_IEEEFP_HAS_FINITE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_C_MATH_HAS_FINITE ";
#ifdef VXL_C_MATH_HAS_FINITE
  vcl_cout << "is set to " << quote(VXL_C_MATH_HAS_FINITE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_C_MATH_HAS_SQRTF ";
#ifdef VXL_C_MATH_HAS_SQRTF
  vcl_cout << "is set to " << quote(VXL_C_MATH_HAS_SQRTF);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_C_MATH_HAS_LROUND ";
#ifdef VXL_C_MATH_HAS_LROUND
  vcl_cout << "is set to " << quote(VXL_C_MATH_HAS_LROUND);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_UNISTD_USLEEP_IS_VOID ";
#ifdef VXL_UNISTD_USLEEP_IS_VOID
  vcl_cout << "is set to " << quote(VXL_UNISTD_USLEEP_IS_VOID);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_HAS_IEEEFP_H ";
#ifdef VXL_HAS_IEEEFP_H
  vcl_cout << "is set to " << quote(VXL_HAS_IEEEFP_H);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC ";
#ifdef VCL_GCC
  vcl_cout << "is set to " << quote(VCL_GCC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_4 ";
#ifdef VCL_GCC_4
  vcl_cout << "is set to " << quote(VCL_GCC_4);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_40 ";
#ifdef VCL_GCC_40
  vcl_cout << "is set to " << quote(VCL_GCC_40);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_41 ";
#ifdef VCL_GCC_41
  vcl_cout << "is set to " << quote(VCL_GCC_41);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_5 ";
#ifdef VCL_GCC_5
  vcl_cout << "is set to " << quote(VCL_GCC_5);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_50 ";
#ifdef VCL_GCC_50
  vcl_cout << "is set to " << quote(VCL_GCC_50);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_51 ";
#ifdef VCL_GCC_51
  vcl_cout << "is set to " << quote(VCL_GCC_51);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_52 ";
#ifdef VCL_GCC_52
  vcl_cout << "is set to " << quote(VCL_GCC_52);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_53 ";
#ifdef VCL_GCC_53
  vcl_cout << "is set to " << quote(VCL_GCC_53);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_6 ";
#ifdef VCL_GCC_6
  vcl_cout << "is set to " << quote(VCL_GCC_6);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_60 ";
#ifdef VCL_GCC_60
  vcl_cout << "is set to " << quote(VCL_GCC_60);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_61 ";
#ifdef VCL_GCC_61
  vcl_cout << "is set to " << quote(VCL_GCC_61);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_62 ";
#ifdef VCL_GCC_62
  vcl_cout << "is set to " << quote(VCL_GCC_62);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_GCC_63 ";
#ifdef VCL_GCC_63
  vcl_cout << "is set to " << quote(VCL_GCC_63);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_ICC ";
#ifdef VCL_ICC
  vcl_cout << "is set to " << quote(VCL_ICC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_ICC_8 ";
#ifdef VCL_ICC_8
  vcl_cout << "is set to " << quote(VCL_ICC_8);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_ICC_80 ";
#ifdef VCL_ICC_80
  vcl_cout << "is set to " << quote(VCL_ICC_80);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_ICC_81 ";
#ifdef VCL_ICC_81
  vcl_cout << "is set to " << quote(VCL_ICC_81);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_WIN32 ";
#ifdef VCL_WIN32
  vcl_cout << "is set to " << quote(VCL_WIN32);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_VC ";
#ifdef VCL_VC
  vcl_cout << "is set to " << quote(VCL_VC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VCL_VC_DOTNET ";
#ifdef VCL_VC_DOTNET
  vcl_cout << "is set to " << quote(VCL_VC_DOTNET);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_WARN_DEPRECATED ";
#ifdef VXL_WARN_DEPRECATED
  vcl_cout << "is set to " << quote(VXL_WARN_DEPRECATED);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_WARN_DEPRECATED_ONCE ";
#ifdef VXL_WARN_DEPRECATED_ONCE
  vcl_cout << "is set to " << quote(VXL_WARN_DEPRECATED_ONCE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "VXL_WARN_DEPRECATED_ABORT ";
#ifdef VXL_WARN_DEPRECATED_ABORT
  vcl_cout << "is set to " << quote(VXL_WARN_DEPRECATED_ABORT);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "CMAKE_DEFINES_VARIABLE_UNIX ";
#ifdef CMAKE_DEFINES_VARIABLE_UNIX
  vcl_cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_UNIX);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "CMAKE_DEFINES_VARIABLE_CYGWIN ";
#ifdef CMAKE_DEFINES_VARIABLE_CYGWIN
  vcl_cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_CYGWIN);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "CMAKE_DEFINES_VARIABLE_MINGW ";
#ifdef CMAKE_DEFINES_VARIABLE_MINGW
  vcl_cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_MINGW);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "CMAKE_DEFINES_VARIABLE_WIN32 ";
#ifdef CMAKE_DEFINES_VARIABLE_WIN32
  vcl_cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_WIN32);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "CMAKE_DEFINES_VARIABLE_APPLE ";
#ifdef CMAKE_DEFINES_VARIABLE_APPLE
  vcl_cout << "is set to " << quote(CMAKE_DEFINES_VARIABLE_APPLE);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__cplusplus ";
#ifdef __cplusplus
  vcl_cout << "is set to " << quote(__cplusplus);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__STDC__ ";
#ifdef __STDC__
  vcl_cout << "is set to " << quote(__STDC__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__STDC_VERSION__ ";
#ifdef __STDC_VERSION__
  vcl_cout << "is set to " << quote(__STDC_VERSION__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "NDEBUG ";
#ifdef NDEBUG
  vcl_cout << "is set to " << quote(NDEBUG);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__STRICT_ANSI__ ";
#ifdef __STRICT_ANSI__
  vcl_cout << "is set to " << quote(__STRICT_ANSI__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__GNUC__ ";
#ifdef __GNUC__
  vcl_cout << "is set to " << quote(__GNUC__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__GNUG__ ";
#ifdef __GNUG__
  vcl_cout << "is set to " << quote(__GNUG__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__GNUC_MINOR__ ";
#ifdef __GNUC_MINOR__
  vcl_cout << "is set to " << quote(__GNUC_MINOR__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__GNUC_PATCHLEVEL__ ";
#ifdef __GNUC_PATCHLEVEL__
  vcl_cout << "is set to " << quote(__GNUC_PATCHLEVEL__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__VERSION__ ";
#ifdef __VERSION__
  vcl_cout << "is set to " << quote(__VERSION__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__OPTIMIZE__ ";
#ifdef __OPTIMIZE__
  vcl_cout << "is set to " << quote(__OPTIMIZE__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__ICC ";
#ifdef __ICC
  vcl_cout << "is set to " << quote(__ICC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__ECC ";
#ifdef __ECC
  vcl_cout << "is set to " << quote(__ECC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__INTEL_COMPILER ";
#ifdef __INTEL_COMPILER
  vcl_cout << "is set to " << quote(__INTEL_COMPILER);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__INTEL_CXXLIB_ICC ";
#ifdef __INTEL_CXXLIB_ICC
  vcl_cout << "is set to " << quote(__INTEL_CXXLIB_ICC);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "_MSC_VER ";
#ifdef _MSC_VER
  vcl_cout << "is set to " << quote(_MSC_VER);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "_COMPILER_VERSION ";
#ifdef _COMPILER_VERSION
  vcl_cout << "is set to " << quote(_COMPILER_VERSION);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__CYGWIN__ ";
#ifdef __CYGWIN__
  vcl_cout << "is set to " << quote(__CYGWIN__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__MINGW32__ ";
#ifdef __MINGW32__
  vcl_cout << "is set to " << quote(__MINGW32__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "_WIN32 ";
#ifdef _WIN32
  vcl_cout << "is set to " << quote(_WIN32);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__alpha__ ";
#ifdef __alpha__
  vcl_cout << "is set to " << quote(__alpha__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__APPLE__ ";
#ifdef __APPLE__
  vcl_cout << "is set to " << quote(__APPLE__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "macintosh ";
#ifdef macintosh
  vcl_cout << "is set to " << quote(macintosh);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "unix ";
#ifdef unix
  vcl_cout << "is set to " << quote(unix);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__unix ";
#ifdef __unix
  vcl_cout << "is set to " << quote(__unix);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__unix__ ";
#ifdef __unix__
  vcl_cout << "is set to " << quote(__unix__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__linux ";
#ifdef __linux
  vcl_cout << "is set to " << quote(__linux);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__linux__ ";
#ifdef __linux__
  vcl_cout << "is set to " << quote(__linux__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__DATE__ ";
#ifdef __DATE__
  vcl_cout << "is set to " << quote(__DATE__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << "__TIME__ ";
#ifdef __TIME__
  vcl_cout << "is set to " << quote(__TIME__);
#else
  vcl_cout << "is not set";
#endif
  vcl_cout << vcl_endl;

  vcl_cout << vcl_endl;

  // These are types that we expect every machine to have.

  testlib_test_begin( "Size of vxl_byte" );
#if VXL_HAS_BYTE
  testlib_test_perform( sizeof( vxl_byte ) == 1 );
#else
  vcl_cout << "(no vxl_byte)" << vcl_endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_uint_8" );
#if VXL_HAS_INT_8
  testlib_test_perform( sizeof( vxl_uint_8 ) == 1 );
#else
  vcl_cout << "(no vxl_uint_8)" << vcl_endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_uint_16" );
#if VXL_HAS_INT_16
  testlib_test_perform( sizeof( vxl_uint_16 ) == 2 );
#else
  vcl_cout << "(no vxl_uint_16)" << vcl_endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_uint_32" );
#if VXL_HAS_INT_32
  testlib_test_perform( sizeof( vxl_uint_32 ) == 4 );
#else
  vcl_cout << "(no vxl_uint_32)" << vcl_endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_ieee_32" );
#if VXL_HAS_IEEE_32
  testlib_test_perform( sizeof( vxl_ieee_32 ) == 4 );
#else
  vcl_cout << "(no vxl_ieee_32)" << vcl_endl;
  testlib_test_perform( false );
#endif

  testlib_test_begin( "Size of vxl_ieee_64" );
#if VXL_HAS_IEEE_64
  testlib_test_perform( sizeof( vxl_ieee_64 ) == 8 );
#else
  vcl_cout << "(no vxl_ieee_64)" << vcl_endl;
  testlib_test_perform( false );
#endif

  vcl_cout << vcl_endl;

  // These may not exist on some platforms.  If they exist, they should
  // be of the correct size.

#if VXL_HAS_INT_64
  testlib_test_begin( "Size of vxl_uint_64" );
  testlib_test_perform( sizeof( vxl_uint_64 ) == 8 );
#else
  vcl_cout << "(no vxl_uint_64)" << vcl_endl;
#endif

#if VXL_HAS_IEEE_96
  testlib_test_begin( "Size of vxl_ieee_96" );
  testlib_test_perform( sizeof( vxl_ieee_96 ) == 12 );
#else
  vcl_cout << "(no vxl_ieee_96)" << vcl_endl;
#endif

#if VXL_HAS_IEEE_128
  testlib_test_begin( "Size of vxl_ieee_128" );
  testlib_test_perform( sizeof( vxl_ieee_128 ) == 16 );
#else
  vcl_cout << "(no vxl_ieee_128)" << vcl_endl;
#endif

  vcl_cout << vcl_endl;

  vcl_cout << "sizeof(char) is " << sizeof(char) << vcl_endl;
  vcl_cout << "sizeof(short) is " << sizeof(short) << vcl_endl;
  vcl_cout << "sizeof(int) is " << sizeof(int) << vcl_endl;
  vcl_cout << "sizeof(long) is " << sizeof(long) << vcl_endl;
  vcl_cout << "sizeof(float) is " << sizeof(float) << vcl_endl;
  vcl_cout << "sizeof(double) is " << sizeof(double) << vcl_endl;
}

TESTMAIN(test_config);