#ifndef vcl_complex_h_
#define vcl_complex_h_

#include "vcl_compiler.h"

// File: vcl_complex.h
//
// The task of this horrible file is to rationalize the complex number
// support in the various compilers.  Basically it promises to give you:
//
// A working vcl_complex<T> template.
//
// Stream >> and << operators.
//
// Instances of the types vcl_complex<float> and vcl_complex<double>
//
// A macro VCL_COMPLEX_INSTANTIATE(T) which allows you to instantiate
// complex over other number types.


#if defined(VCL_STLPORT)
# include "stlport/vcl_complex.h"
// ---------- Visual Studio 9
#elif defined(VCL_VC_9)
# include "win32-vc9/vcl_complex.h"

// ---------- SunPro compiler
#elif defined(VCL_SUNPRO_CC)
# include "sunpro/vcl_complex.h"

// ---------- ISO
#else
# include "iso/vcl_complex.h"
#endif


# if !VCL_COMPLEX_POW_WORKS
#  undef vcl_pow
#  define vcl_pow vcl_pow
// several implementations of pow are wrong.
// e.g. pow(complex<double>(-1.0,0.0), 0.5) returns (Nan, 0) rather than (0,1).

template <class T> inline vcl_complex<T>
  vcl_pow(const vcl_complex<T>& xin, int y)
{
  vcl_complex<T> r = 1.0;
  vcl_complex<T> x = xin;
  if (y < 0) {
    y = -y;
    x = ((T)1)/x; }
  while (y) {
    if (y & 1)   r *= x;
    if (y >>= 1) x *= x; }
  return r;
}

template <class T> inline vcl_complex<T>
  vcl_pow(const vcl_complex<T>& x, const T& y)
{
  return vcl_exp(y * vcl_log(x));
}

template <class T> inline vcl_complex<T>
  vcl_pow(const T& x, const vcl_complex<T>& y)
{
  return vcl_exp(y * vcl_log(vcl_complex<T>(x, T(0))));
}

template <class T> inline vcl_complex<T>
  vcl_pow(const vcl_complex<T>& x, const vcl_complex<T>& y)
{
  return vcl_exp(y * vcl_log(x));
}

# endif // !VCL_COMPLEX_POW_WORKS


#if 0
// this breaks the sunpro build. it should be moved so that
// it is only seen by compilers that need it. - fsm.
inline vcl_complex<double> operator*(float c, vcl_complex<double> const &z) { return z * (double)c; }
inline vcl_complex<double> operator*(vcl_complex<double> const &z, float c) { return z * (double)c; }
inline vcl_complex<float > operator*(double c, vcl_complex<float> const &z) { return z * (float)c; }
inline vcl_complex<float > operator*(vcl_complex<float> const &z, double c) { return z * (float)c; }
inline vcl_complex<double> operator/(vcl_complex<double> const &z, float c) { return z / (double)c; }
inline vcl_complex<float > operator/(vcl_complex<float> const &z, double c) { return z / (float)c; }
#endif

//--------------------------------------------------------------------------------

// bogus instantiation macro.
#define VCL_COMPLEX_INSTANTIATE(T) extern "you must include vcl_complex.txx instead"

#include "vcl_complex.txx"

#endif // vcl_complex_h_
