#ifndef vcl_cassert_h_
#define vcl_cassert_h_
// .NAME vcl_cassert.h
// .INCLUDE vcl/vcl_cassert.h
// .FILE vcl_cassert.cxx
// @author fsm@robots.ox.ac.uk

#include <vcl/vcl_compiler.h>

#include <assert.h>

// fsm: There should not be a vcl_assert macro as there is no
// std::assert symbol. If your assert macro is broken, fix it
// here using #undef and #define.
//#define vcl_assert(x) assert(x)

#endif // vcl_cassert_h_
