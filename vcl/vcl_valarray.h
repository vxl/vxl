#ifndef vcl_valarray_h_
#define vcl_valarray_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_compiler.h>

#if defined(VCL_GCC)
# include <valarray> // 2.95
# define vcl_valarray valarray

#else
# include <valarray>
# define vcl_valarray std::valarray
#endif

#endif
