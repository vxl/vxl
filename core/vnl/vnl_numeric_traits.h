#ifndef vnl_numeric_traits_h_
#define vnl_numeric_traits_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vnl/vnl_numeric_traits.h

//: \file
//  \brief Templated zero/one/precision
//
//  To allow templated numerical algorithms to determine appropriate
//    values for zero, one, maxval, and types for double precision,
//    maximum product etc.
//
//  \author Andrew W. Fitzgibbon, Oxford RRG
//  \date   04 Sep 96
//
// \verbatim
//     Modifications:
//     980212 AWF Initial version.
//     AWF 010498 Moved to math
//     LSB (Manchester) 23/3/01 Documentation tidied
//     Peter Vanroose   14/7/01 vnl_rational added
//     Peter Vanroose   14/10/01 vnl_rational moved to vnl_rational.h
//     AWF 250202 Add const T specializations for the basic types.
// \endverbatim
//
//-----------------------------------------------------------------------------

#include <vcl_complex_fwd.h>

// this is an empty class template.
// only the specializations make sense.
template <class T>
class vnl_numeric_traits;
#if 0
{
  //: Additive identity
  static const T zero;

  //: Multiplicative identity
  static const T one;

  //: Return value of abs()
  typedef T abs_t;

  //: Name of a type twice as long as this one for accumulators and products.
  typedef /* long */ double double_t;

  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};
#endif

#ifndef NO_STD_BOOL
VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<bool> {
public:
  //: Additive identity
  static const bool zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const bool one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned int abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef unsigned int double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};
#endif

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<char> {
public:
  //: Additive identity
  static const char zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const char one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned char abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef short double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<char const> : public vnl_numeric_traits<char> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned char> {
public:
  //: Additive identity
  static const unsigned char zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const unsigned char one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned char abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef unsigned short double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned char const> : public vnl_numeric_traits<unsigned char> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed char> {
public:
  //: Additive identity
  static const signed char zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const signed char one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned char abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef signed short double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed char const> : public vnl_numeric_traits<signed char> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned short> {
public:
  //: Additive identity
  static const unsigned short zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const unsigned short one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned short abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef unsigned int double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned short const> : public vnl_numeric_traits<unsigned short> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed short> {
public:
  //: Additive identity
  static const signed short zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const signed short one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned short abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef signed int double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed short const> : public vnl_numeric_traits<signed short> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned int> {
public:
  //: Additive identity
  static const unsigned int zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const unsigned int one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned int abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef unsigned int double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned int const> : public vnl_numeric_traits<unsigned int> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed int> {
public:
  //: Additive identity
  static const signed int zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const signed int one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned int abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef signed int double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed int const> : public vnl_numeric_traits<signed int> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed long> {
public:
  //: Additive identity
  static const signed long zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const signed long one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned long abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef signed long double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<signed long const> : public vnl_numeric_traits<signed long > {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned long> {
public:
  //: Additive identity
  static const unsigned long zero VCL_STATIC_CONST_INIT_INT(0);
  //: Multiplicative identity
  static const unsigned long one VCL_STATIC_CONST_INIT_INT(1);
  //: Return value of abs()
  typedef unsigned long abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef unsigned long double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<unsigned long const> : public vnl_numeric_traits<unsigned long> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<float> {
public:
  //: Additive identity
  static const float zero VCL_STATIC_CONST_INIT_FLOAT(0.0F);
  //: Multiplicative identity
  static const float one VCL_STATIC_CONST_INIT_FLOAT(1.0F);
  //: Return value of abs()
  typedef float abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef double double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<float const> : public vnl_numeric_traits<float> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<double> {
public:
  //: Additive identity
  static const double zero VCL_STATIC_CONST_INIT_FLOAT(0.0);
  //: Multiplicative identity
  static const double one VCL_STATIC_CONST_INIT_FLOAT(1.0);
  //: Return value of abs()
  typedef double abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef long double double_t;
  //: Name of type which results from multiplying this type with a double
  typedef double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<double const> : public vnl_numeric_traits<double> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<long double> {
public:
  //: Additive identity
  static const long double zero VCL_STATIC_CONST_INIT_FLOAT(0.0);
  //: Multiplicative identity
  static const long double one VCL_STATIC_CONST_INIT_FLOAT(1.0);
  //: Return value of abs()
  typedef long double abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef long double double_t; // ahem
  //: Name of type which results from multiplying this type with a double
  typedef long double real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<long double const> : public vnl_numeric_traits<long double> {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits< vcl_complex<float> > {
public:
  //: Additive identity
  static const vcl_complex<float> zero;
  //: Multiplicative identity
  static const vcl_complex<float> one;
  //: Return value of abs()
  typedef float abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef vcl_complex<vnl_numeric_traits<float>::double_t> double_t;
  //: Name of type which results from multiplying this type with a double
  typedef vcl_complex<float> real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<vcl_complex<float> const> : public vnl_numeric_traits<vcl_complex<float> > {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits< vcl_complex<double> > {
public:
  //: Additive identity
  static const vcl_complex<double> zero;
  //: Multiplicative identity
  static const vcl_complex<double> one;
  //: Return value of abs()
  typedef double abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef vcl_complex<vnl_numeric_traits<double>::double_t> double_t;
  //: Name of type which results from multiplying this type with a double
  typedef vcl_complex<double> real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<vcl_complex<double> const> : public vnl_numeric_traits<vcl_complex<double> > {
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits< vcl_complex<long double> > {
public:
  //: Additive identity
  static const vcl_complex<long double> zero;
  //: Multiplicative identity
  static const vcl_complex<long double> one;
  //: Return value of abs()
  typedef long double abs_t;
  //: Name of a type twice as long as this one for accumulators and products.
  typedef vcl_complex<vnl_numeric_traits<long double>::double_t> double_t;
  //: Name of type which results from multiplying this type with a double
  typedef vcl_complex<long double> real_t;
};

VCL_DEFINE_SPECIALIZATION
class vnl_numeric_traits<vcl_complex<long double> const> : public vnl_numeric_traits<vcl_complex<long double> > {
};

#endif // vnl_numeric_traits_h_
