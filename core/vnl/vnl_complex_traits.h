// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
#ifndef vnl_complex_traits_h_
#define vnl_complex_traits_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_complex_traits
//
// .SECTION Description
//    To allow templated real or complex algorithms to determine appropriate
//    actions of conjugation, complexification etc.

//
// .NAME        vnl_complex_traits
// .FILE        vnl/vnl_complex_traits.cxcxx
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

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_complex_traits.
