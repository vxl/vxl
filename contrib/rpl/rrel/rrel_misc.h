#ifndef rrel_misc_h_
#define rrel_misc_h_
//:
// \file
// \author Chuck Stewart
// \date   Summer 2001

//: Chebychev approximation to erfc.
//  (Taken from "Numerical Recipes in C".)
extern double rrel_misc_erfcc( double x );

//: Inverse of the Gaussian CDF.
//  Provided by Robert W. Cox from the Biophysics Research Institute
//  at the Medical College of Wisconsin. This function is based off of
//  a rational polynomial approximation to the inverse Gaussian CDF
//  which can be found in
//
//  M. Abramowitz and I.A. Stegun. Handbook of Mathematical Functions
//  with Formulas, Graphs, and Mathematical Tables.  John Wiley &
//  Sons.  New York. Equation 26.2.23. pg. 933. 1972.
extern double rrel_misc_gaussian_cdf_inv( double p );

#endif
