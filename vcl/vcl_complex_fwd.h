//-*- c++ -*-------------------------------------------------------------------
#ifndef vcl_complex_fwd_h_
#define vcl_complex_fwd_h_

/*
  fsm@robots.ox.ac.uk \and awf@robots.ox.ac.uk
*/

// Purpose: Forward declare the template class known
// as vcl_complex<T> without the cost of including the
// header file. Also declare the two typedefs at the
// bottom.
// *** make sure this file is consistent with vcl_complex.h ***
//
// Note: the standard complex class is just std::complex<T>, so
// on an ISO compiler, no forward declaration header is needed.

#include "vcl_compiler.h"

// ---------- emulation
#if !VCL_USE_NATIVE_COMPLEX
# include "emulation/vcl_complex_fwd.h"

// ---------- egcs
#elif defined(VCL_EGCS)
template <class T> class complex;
# define vcl_complex complex

// ---------- gcc 2.95
#elif defined(VCL_GCC_295)
# if !defined(GNU_LIBSTDCXX_V3)
// old library
   template <class T> class complex;
#  define vcl_complex complex
# else
// new library (broken)
#  include "emulation/vcl_complex_fwd.h"
# endif

// ---------- native WIN32
#elif defined(VCL_WIN32)
# include "win32/vcl_complex_fwd.h"

// ---------- SunPro compiler
#elif defined(VCL_SUNPRO_CC)
# include "sunpro/vcl_complex_fwd.h"

// ---------- ISO
#else
namespace std {
  template <typename T> class complex;
}
# ifndef vcl_complex
# define vcl_complex std::complex
# endif
#endif

// by definition, this should work for all compilers :
typedef vcl_complex<float>  vcl_float_complex;
typedef vcl_complex<double> vcl_double_complex;

#endif // vcl_complex_fwd_h_
