#ifndef vnl_complex_traits_h_
#define vnl_complex_traits_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// .NAME        vnl_complex_traits
// .LIBRARY     vnl
// .HEADER      Numerics Package
// .INCLUDE     vnl/vnl_complex_traits.h
// .FILE        vnl/vnl_complex_traits.cxx
//
// .SECTION Description
//    To allow templated real or complex algorithms to determine appropriate
//    actions of conjugation, complexification etc.
//
// .SECTION Author
//     F. Schaffalitzky, Oxford RRG, 26 Mar 1999
//
// .SECTION Modifications:
//
//-----------------------------------------------------------------------------

#include <vcl/vcl_compiler.h>
#include <vnl/vnl_complex.h>

// default is real.
template <class T>
class vnl_complex_traits {
public:
  // -- whether complex or not
  enum nd { isreal = true };

  // -- complex conjugation
  static T conjugate(T x) { return x; }

  // -- complexification
  static vnl_complex<T> complexify(T x) { return vnl_complex<T>(x,T(0)); }
};

// -- override for vnl_complex<REAL> :

// vnl_complex<float>
VCL_DECLARE_SPECIALIZATION(class vnl_complex_traits< vnl_complex<float> >)

class vnl_complex_traits< vnl_complex<float> > {
public:
  enum nd { isreal = false };
  static vnl_complex<float> conjugate(vnl_complex<float> z) { return vnl_complex<float>(z.real(), -z.imag()); }
  static vnl_complex<float> complexify(vnl_complex<float> z) { return z; }
};

// vnl_complex<double>
VCL_DECLARE_SPECIALIZATION(class vnl_complex_traits< vnl_complex<double> >)

class vnl_complex_traits< vnl_complex<double> > {
public:
  enum nd { isreal = false };
  static vnl_complex<double> conjugate(vnl_complex<double> z) { return vnl_complex<double>(z.real(), -z.imag()); }
  static vnl_complex<double> complexify(vnl_complex<double> z) { return z; }
};

#endif // vnl_complex_traits_h_
