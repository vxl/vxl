#ifndef vcl_complex_h_
#define vcl_complex_h_

#include <vcl/vcl_compiler.h>
#include <vcl/vcl_cmath.h>

/** File: vcl_complex.h
 
The task of this horrible file is to rationalize the complex number
support in the various compilers.  Basically it promises to give you:
A working vcl_complex<T> template.
Stream >> and << operators
Instances of the types vcl_complex<float> and vcl_complex<double>

A macro VCL_COMPLEX_INSTANTIATE(T) which allows you to instantiate
complex over other number types.
*/

#if VCL_USE_NATIVE_COMPLEX
# ifdef __GNUC__
#  include <complex.h>
#  define vcl_complex complex
# elif defined(VCL_WIN32)
// fsm@robots: complex<T> is derived from _Complex_base<T>, which is not
// declared with __declspec(dllimport). So complex<T> shouldn't be either
// (else the compiler will emit an error). Whether or not it is depends on
// the value of _CRTIMP being set, e.g. in <math.h>
#  undef _CRTIMP
//#  define _CRTIMP __declspec(dllimport)
#  define _CRTIMP
#  include <complex>
#  define vcl_complex std::complex
template <class T> inline T norm(vcl_complex<T> const &z) { return z.real()*z.real() + z.imag()*z.imag(); }
template <class T> inline T abs(vcl_complex<T> const &z) { return sqrt(norm(z)); }
#  include <vcl/vcl_iostream.h>
template <class T> inline ostream &operator<<(ostream &os, vcl_complex<T> const &z) 
  { return os << '(' << z.real() << ',' << ')'; }
template <class T> inline istream &operator>>(istream &is, vcl_complex<T> &z) 
{ abort(); T r(0), i(0); is >> r >> i; z = vcl_complex<T>(r,i); return is; } // FIXME
# else
// eg sunpro
#  include <vcl/vcl_function.h> // get !=(T,T) declared. so that !=(complex<T>..) specializes
#  include <complex>
#  define vcl_complex std::complex

#  ifdef VCL_SUNPRO_CC_50
// Override these, as the SunPro compiler doesn't declare them as
// partial specializations.
inline bool operator!=(const vc_complex<double>& a, const vcl_complex<double>& b) {
  return !(a == b);
}
inline bool operator!=(const complex<float>& a, const complex<float>& b) {
  return !(a == b);
}
// Add this for SunPro.  The const& on the double arg appears to be
// necessary to avoid an overloading conflict with complex<double> * double.
inline complex<double> operator*(vcl_complex<float> const& a, const double& b) {
  return vcl_complex<double>(a.real () * b, a.imag() * b);
}
inline complex<double> operator*(double const& b, vcl_complex<float> const& a) {
  return vcl_complex<double>(a.real () * b, a.imag() * b);
}
#  endif // VCL_SUNPRO_CC_50

# endif
#else
// all emulation :
# include <vcl/emulation/vcl_complex.h>
#endif


// At this point we have vcl_complex<T>, so this should work for all compilers :
typedef vcl_complex<double> vcl_double_complex;
typedef vcl_complex<float> vcl_float_complex;

#endif
