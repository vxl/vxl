// This is brl/bbas/bsta/bsta_gauss_f1.h
#ifndef bsta_gauss_f1_h_
#define bsta_gauss_f1_h_
//:
// \file
// \brief A Gaussian in 1D with type float
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date June 13, 2006
//
// Note: the gaussian sphere implementation is most efficient
//       for the univariate case

#include "bsta_gaussian_sphere.h"

typedef bsta_gaussian_sphere<float,1> bsta_gauss_f1;

#endif // bsta_gauss_f1_h_
