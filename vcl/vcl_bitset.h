#ifndef vcl_bitset_h_
#define vcl_bitset_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>

#if defined(VCL_GCC)
# include <bitset> // 2.95
# define vcl_bitset bitset

#else
# include <vcl/iso/vcl_bitset.h>
#endif

#endif
