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

#include <vcl/vcl_compiler.h>

// ---------- emulation
#if !VCL_USE_NATIVE_COMPLEX
template <class T> class vcl_complex;

// ---------- native gcc
#elif defined(__GNUC__)
template <class T> class complex;
# define vcl_complex complex

// ---------- native WIN32
#elif defined(VCL_WIN32)
// forward declare the native std::complex<T> :
namespace std { template <class T> class complex; }
using std::complex;
# define vcl_complex complex

// ---------- SunPro compiler
#elif defined(VCL_SUNPRO_CC)
// forward declare the native std::complex<T> :
namespace std { template <class T> class complex; }
# define vcl_complex std::complex

// ---------- all other compilers
#else
// this will work, but may be costly :
# include <vcl/vcl_complex.h>
#endif

// by definition, this should work for all compilers :
typedef vcl_complex<float>  vcl_float_complex;
typedef vcl_complex<double> vcl_double_complex;

#endif
