// The <cassert> header does not
// have inclusion guards. The
// C and C++ standards say so.

// .NAME vcl_cassert
// .INCLUDE vcl_cassert.h
// .FILE vcl_cassert.cxx
// @author fsm@robots.ox.ac.uk

#include "vcl_compiler.h"

#if defined(VCL_GCC) && !defined(GNU_LIBSTDCXX_V3)
# include <assert.h>
#elif defined(VCL_SGI_CC_720)
# include <assert.h>
#elif defined(GNU_LIBSTDCXX_V3)
// The "program"
//   #include <cassert>
//   #include <iostream>
// breaks v3.
extern void vcl_cassert_failure(char const *, int, char const *);
# undef assert
# define assert(x) do { if (!(x)) vcl_cassert_failure(__FILE__, __LINE__, #x); } while (false)
#else
# include "iso/vcl_cassert.h"
#endif

// fsm: There should not be a vcl_assert macro as there is no
// std::assert symbol. If your assert macro is broken, fix it
// here using #undef and #define.
//#define vcl_assert(x) assert(x)
