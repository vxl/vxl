// This is brl/bbas/bsta/bsta_gauss_f2.h
#ifndef bsta_gauss_f2_h_
#define bsta_gauss_f2_h_
//:
// \file
// \brief A Gaussian in 2D with type float
// \author Ozge C. Ozcanli
// \date 20 October 2008
//
// Note: the gaussian sphere implementation is most efficient
//       for the univariate case

#include "bsta_gaussian_sphere.h"

typedef bsta_gaussian_sphere<float,2> bsta_gauss_f2;

#endif // bsta_gauss_f2_h_
