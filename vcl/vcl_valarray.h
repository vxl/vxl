#ifndef vcl_valarray_h_
#define vcl_valarray_h_
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>

#if defined(VCL_GCC)
# include <valarray> // 2.95
# define vcl_valarray valarray

#else
# include <vcl/iso/vcl_valarray.h>
#endif

#endif
