#ifdef __GNUC__
#pragma implementation
#endif

// This is vxl/vnl/vnl_real_polynomial.cxx

//:
// \file
// \brief Evaluation of real polynomials - the implemenation
// \author Andrew W. Fitzgibbon, Oxford RRG 23 Aug 96
//
// Modifications
// IMS (Manchester) 14/03/2001: Added Manchester IO scheme

#include "vnl_real_polynomial.h"
#include <vcl_iostream.h>
#include <vcl_complex.h>

// This is replacing a member template...
template <class T>
T vnl_real_polynomial_evaluate(double const *a, int n, T const& x)
{
  --n;
  T acc = a[n];
  T xn = x;
  do {
    acc += a[--n] * xn;
    xn *= x;
  } while (n);

  return acc;
}

#ifdef WIN32
#define SELECT(T) <T >
#else
#define SELECT(T)
#endif

//: Instantiate templates before use
template double vnl_real_polynomial_evaluate SELECT(double )
      (double const*,int,double const&);
template vcl_complex<double> vnl_real_polynomial_evaluate SELECT(vcl_complex<double>)
      (double const*,int,vcl_complex<double> const&);

//: Evaluate polynomial at value x
double vnl_real_polynomial::evaluate(double x) const
{
  return vnl_real_polynomial_evaluate SELECT(double)(coeffs_.data_block(), coeffs_.size(), x);
}


//: Evaluate polynomial at complex value x
vcl_complex<double> vnl_real_polynomial::evaluate(vcl_complex<double> const& x) const
{
  return vnl_real_polynomial_evaluate SELECT(vcl_complex<double>)
     (coeffs_.data_block(), coeffs_.size(), x);
}

//: Evaluate derivative at value x. Not implemented.
double vnl_real_polynomial::devaluate(double /*x*/) const
{
  return HUGE_VAL;
}


//: Evaluate derivative at complex value x. Not implemented.
vcl_complex<double> vnl_real_polynomial::devaluate(vcl_complex<double> const& /*x*/) const
{
  return HUGE_VAL;
}


