#ifndef vcl_utility_h_
#define vcl_utility_h_

#include <vcl/vcl_compiler.h>

#if !VCL_USE_NATIVE_STL
# include <vcl/emulation/vcl_utility.h>
# define vcl_make_pair make_pair

#elif defined(__GNUC__)
# include <utility>
# define vcl_pair pair
# define vcl_make_pair make_pair

#else // e.g. VCL_SUNPRO_CC or VCL_VC60
# include <utility>
# define vcl_pair std::pair
# define vcl_make_pair std::make_pair
#endif

#define VCL_PAIR_INSTANTIATE extern "you must include vcl/vcl_utility.txx first"
#define VCL_PAIR_const_INSTANTIATE extern "you must include vcl/vcl_utility.txx first"

#endif
