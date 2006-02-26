#ifndef BNL_ALGO_LEGENDRE_POLYNOMIAL_H_
#define BNL_ALGO_LEGENDRE_POLYNOMIAL_H_
//:
//\file
//\brief Evalue a real Legendre polynomial by recursive equation
//\author Kongbin Kang, Brown University
//\date Dec. 21th, 2004.

#include <vnl/vnl_real_polynomial.h>

//: generate a n-th degree legendre polynomial
vnl_real_polynomial bnl_legendre_polynomial(int n);

#endif
