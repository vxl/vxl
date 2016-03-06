#ifndef vcl_set_h_
#define vcl_set_h_

#include "vcl_compiler.h"

// vcl_less<> is a default argument to vcl_set<> and vcl_multiset<>
// so we need this for compilers where vcl_less is a macro.
#include "vcl_functional.h"
#include "iso/vcl_set.h"

#endif // vcl_set_h_
