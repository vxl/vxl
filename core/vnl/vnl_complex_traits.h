// This is vxl/vnl/vnl_complex_traits.h
#ifndef vnl_complex_traits_h_
#define vnl_complex_traits_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
//  \brief To allow templated algorithms to determine appropriate actions of conjugation, complexification etc.
//  \author F. Schaffalitzky, Oxford RRG, 26 Mar 1999
//
// \verbatim
// Modifications:
// LSB (Manchester) 26/3/01 Documentation tidied
// \endverbatim
//-----------------------------------------------------------------------------

#include <vcl_complex.h>

#if 0 // The old implementation
  //: Whether complex or not
  enum { isreal = true };

  //: Complex conjugation.
  static T conjugate(T x);

  //: Complexification.
  static vcl_complex<T> complexify(T x);
#endif

template <class T> // the primary template is empty, by design.
struct vnl_complex_traits;

#define macro(T) \
VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<T > \
{ \
  enum { isreal = true }; \
  static T conjugate(T x) { return x; } \
  static vcl_complex<T> complexify(T x) { return vcl_complex<T >(x, (T)0); } \
}
#define makro(T) \
macro(signed T); \
macro(unsigned T)
makro(char);
makro(int);
makro(long);
#undef makro
#undef macro


VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<float>
{
  enum { isreal = true };
  static float conjugate(float x) { return x; }
  static vcl_complex<float> complexify(float x) { return vcl_complex<float>(x, 0.0f); }
};

VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<double>
{
  enum { isreal = true };
  static double conjugate(double x) { return x; }
  static vcl_complex<double> complexify(double x) { return vcl_complex<double>(x, 0.0); }
};

VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<long double>
{
  enum { isreal = true };
  static long double conjugate(long double x) { return x; }
  static vcl_complex<long double> complexify(long double x) { return vcl_complex<long double>(x, 0.0); }
};

VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<vcl_complex<float> >
{
  enum { isreal = false };
  static vcl_complex<float> conjugate(vcl_complex<float> x) { return vcl_conj(x); }
  static vcl_complex<float> complexify(float x) { return x; }
};

VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<vcl_complex<double> >
{
  enum { isreal = false };
  static vcl_complex<double> conjugate(vcl_complex<double> x) { return vcl_conj(x); }
  static vcl_complex<double> complexify(double x) { return x; }
};

VCL_DEFINE_SPECIALIZATION struct vnl_complex_traits<vcl_complex<long double> >
{
  enum { isreal = false };
  static vcl_complex<long double> conjugate(vcl_complex<long double> x) { return vcl_conj(x); }
  static vcl_complex<long double> complexify(long double x) { return x; }
};

#endif // vnl_complex_traits_h_
