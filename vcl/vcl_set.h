#ifndef vcl_set_h_
#define vcl_set_h_

#include <vcl/vcl_compiler.h>

#define VCL_SET_INSTANTIATE extern "you must include vcl/vcl_set.txx first"

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_set.h>
# include <vcl/emulation/vcl_multiset.h>

#elif defined(__GNUC__)
# include <set>
# include <multiset.h>
# define vcl_set set
# define vcl_multiset multiset

#else
# include <set>
# define vcl_set std::set
# define vcl_multiset std::multiset
#endif

#endif
