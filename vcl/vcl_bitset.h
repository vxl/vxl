#ifndef vcl_bitset_h_
#define vcl_bitset_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>

#if defined(VCL_GCC)
// 2.95
# include <bitset>
# define vcl_bitset bitset

#else
# include <bitset>
# define vcl_bitset std::bitset
#endif

#endif
