#ifndef vcl_complex_h_
#define vcl_complex_h_

#include <vcl/vcl_compiler.h>

// File: vcl_complex.h
// 
// The task of this horrible file is to rationalize the complex number
// support in the various compilers.  Basically it promises to give you:
//
// A working vcl_complex<T> template.
//
// The typedefs vcl_float_complex, vcl_double_complex.
//
// Stream >> and << operators.
//
// Instances of the types vcl_complex<float> and vcl_complex<double>
//
// A macro VCL_COMPLEX_INSTANTIATE(T) which allows you to instantiate
// complex over other number types.
//
// If you just want to forward declare the vcl complex types, use 
// vcl_complex_fwd.h instead.
//  *** make sure the two files are kept consistent ***

// ---------- all emulation
#if !VCL_USE_NATIVE_COMPLEX 
# include <vcl/emulation/vcl_complex.h>

// ---------- native gcc
#elif defined(__GNUC__)
# include <complex>
# define vcl_complex complex

// ---------- native WIN32
#elif defined(VCL_WIN32)
// fsm@robots: complex<T> is derived from _Complex_base<T>, which is not
// declared with __declspec(dllimport). So complex<T> shouldn't be either
// (else the compiler will emit an error). Whether or not it is depends on
// the value of _CRTIMP being set, e.g. in <math.h>
# include <vcl/vcl_iostream.h>
# undef _CRTIMP
# define _CRTIMP
# include <complex>
# define vcl_complex complex
using std :: complex;
using std :: sqrt;
using std :: arg;
using std :: polar;
using std :: conj;
using std :: real;
using std :: imag;
// now for the stream operators :
template <class T> inline
T norm(vcl_complex<T> const &z) 
{ return z.real()*z.real() + z.imag()*z.imag(); }
template <class T> inline
T abs(vcl_complex<T> const &z)
{ return sqrt(norm(z)); }
// these were needed with old iostreams, they cause ambiguity now
// template <class T> inline 
// vcl_ostream &operator<<(vcl_ostream &os, vcl_complex<T> const &z)
// { return os << '(' << z.real() << ',' << ')'; }
//template <class T> inline 
//vcl_istream &operator>>(vcl_istream &is, vcl_complex<T> &z) 
//{ abort(); T r(0), i(0); is >> r >> i; z = vcl_complex<T>(r,i); return is; } // FIXME

// ---------- SunPro compiler
#elif defined(VCL_SUNPRO_CC)
# include <vcl/vcl_functional.h> // get !=(T,T) declared. so that !=(complex<T>..) specializes
# include <complex>              // get compiler <complex>
# define vcl_complex std::complex
// Override these, as the SunPro compiler doesn't declare them as
// partial specializations.
inline bool operator!=(const vcl_complex<double>& a, const vcl_complex<double>& b) {
  return !(a == b);
}
inline bool operator!=(const vcl_complex<float>& a, const vcl_complex<float>& b) {
  return !(a == b);
}
// The const& on the double arg appears to be necessary to avoid an overloading
// conflict with complex<double> * double.
inline complex<double> operator*(vcl_complex<float> const& a, const double& b) {
  return vcl_complex<double>(a.real () * b, a.imag() * b);
}
inline complex<double> operator*(double const& b, vcl_complex<float> const& a) {
  return vcl_complex<double>(a.real () * b, a.imag() * b);
}

// ---------- all other compilers (eg ISO ones... :)
#else
# include <complex>
# define vcl_complex std::complex

#endif

//--------------------------------------------------------------------------------

// At this point we have vcl_complex<T>, so this should work for all compilers :
typedef vcl_complex<double> vcl_double_complex;
typedef vcl_complex<float> vcl_float_complex;

#endif
