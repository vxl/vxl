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
#ifndef vnl_numeric_traits_h_
#define vnl_numeric_traits_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_numeric_traits
//
// .SECTION Description
//    To allow templated numerical algorithms to determine appropriate
//    values for zero, one, maxval, and types for double precision,
//    maximum product etc.
//
// .NAME        vnl_numeric_traits - Templated zero/one/precision
// .LIBRARY     vnl
// .HEADER      Numerics Package
// .INCLUDE     vnl/numeric_traits.h
// .FILE        vnl/numeric_traits.ccxx
//
// .SECTION Description
//    To allow templated numerical algorithms to determine appropriate
//    values for zero, one, maxval, and types for double precision,
//    maximum product etc.
//
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 04 Sep 96
//
// .SECTION Modifications:
//     AWF 010498 Moved to math
//
//-----------------------------------------------------------------------------


#include <vcl/vcl_compiler.h>
//#include <vnl/vnl_complex.h>

template <class T>
class vnl_numeric_traits {
public:

// -- Additive identity
  static const T zero;

// -- Multiplicative identity
  static const T one;

// -- Return value of abs()
  typedef T abs_t; 

// -- Name of a type twice as long as this one for accumulators and products.
// For float it's double, for vnl_complex<float> it's vnl_complex<double>.
// In theory one might use long double for double, but in practice it's too slow
// and it doesn't matter so much for double.  Define NUMERICS_LONG_DOUBLE_IS_FAST
// if you want long double.
  typedef double double_t; 

// -- Name of type which results from multiplying this type with a double
  typedef double real_t;

};

#if defined(i386)
// 16 bit int
typedef short long_char;
typedef long int long_short;
typedef long int long_int;
typedef unsigned short long_uchar;
typedef unsigned long int long_ushort;
typedef unsigned long long long_uint;
#else
// 32 bit 
typedef short long_char;
typedef int long_short;
typedef long long_int;
typedef unsigned short long_uchar;
typedef unsigned int long_ushort;
typedef unsigned long long_uint;
#endif

#ifndef NO_STD_BOOL
class vnl_numeric_traits<bool> {
public:
  static const bool zero VCL_STATIC_CONST_INIT_INT(0);
  static const bool one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned int abs_t;
  typedef unsigned int double_t;
  typedef double real_t;
};
#endif

class vnl_numeric_traits<unsigned char> {
public:
  static const unsigned char zero VCL_STATIC_CONST_INIT_INT(0);
  static const unsigned char one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned char abs_t;
  typedef unsigned short double_t;
  typedef double real_t;
};

class vnl_numeric_traits<signed char> {
public:
  static const signed char zero VCL_STATIC_CONST_INIT_INT(0);
  static const signed char one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned char abs_t;
  typedef signed short double_t;
  typedef double real_t;
};

class vnl_numeric_traits<unsigned short> {
public:
  static const unsigned short zero VCL_STATIC_CONST_INIT_INT(0);
  static const unsigned short one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned short abs_t;
  typedef unsigned int double_t;
  typedef double real_t;
};

class vnl_numeric_traits<signed short> {
public:
  static const signed short zero VCL_STATIC_CONST_INIT_INT(0);
  static const signed short one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned short abs_t;
  typedef signed int double_t;
  typedef double real_t;
};

class vnl_numeric_traits<unsigned int> {
public:
  static const unsigned int zero VCL_STATIC_CONST_INIT_INT(0);
  static const unsigned int one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned int abs_t;
  typedef unsigned int double_t;
  typedef double real_t;
};

class vnl_numeric_traits<signed int> {
public:
  static const signed int zero VCL_STATIC_CONST_INIT_INT(0);
  static const signed int one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned int abs_t;
  typedef signed int double_t;
  typedef double real_t;
};

class vnl_numeric_traits<signed long> {
public:
  static const signed long zero VCL_STATIC_CONST_INIT_INT(0);
  static const signed long one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned long abs_t;
  typedef signed long double_t;
  typedef double real_t;
};

class vnl_numeric_traits<unsigned long> {
public:
  static const unsigned long zero VCL_STATIC_CONST_INIT_INT(0);
  static const unsigned long one VCL_STATIC_CONST_INIT_INT(1);
  typedef unsigned long abs_t;
  typedef unsigned long double_t;
  typedef double real_t;
};

class vnl_numeric_traits<float> {
public:
  static const float zero VCL_STATIC_CONST_INIT_FLOAT(0.0F);
  static const float one VCL_STATIC_CONST_INIT_FLOAT(1.0F);
  typedef float abs_t;
  typedef double double_t;
  typedef double real_t;
};

class vnl_numeric_traits<double> {
public:
  static const double zero VCL_STATIC_CONST_INIT_FLOAT(0.0);
  static const double one VCL_STATIC_CONST_INIT_FLOAT(1.0);
  typedef double abs_t;
  typedef VCL_long_double double_t;
  typedef double real_t;
};

#define VNL_USED_COMPLEX
#ifdef VNL_COMPLEX_AVAILABLE
// See vnl_complex.h

// G++ barfs if the specializations are themselves templated
// declaring the statics "const" crashes 2.7.2

class vnl_numeric_traits< vnl_complex<float> > {
public:
  static vnl_complex<float> zero;
  static vnl_complex<float> one;
  typedef float abs_t;
  typedef vnl_complex<vnl_numeric_traits<float>::double_t> double_t;
  typedef vnl_complex<float> real_t;
};

class vnl_numeric_traits< vnl_complex<double> > {
public:
  static vnl_complex<double> zero;
  static vnl_complex<double> one;
  typedef double abs_t;
  typedef vnl_complex<vnl_numeric_traits<double>::double_t> double_t;
  typedef vnl_complex<double> real_t;
};

#endif

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_numeric_traits.
