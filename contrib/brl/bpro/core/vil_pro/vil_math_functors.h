#ifndef vil_math_functors_h_
#define vil_math_functors_h_
//:
// \file
// \brief Additional vil_math functors
// \author J.L. Mundy
// \date February 20, 2008
//
// \verbatim
//  Modifications
// \endverbatim
//
#include <vcl_cmath.h>

//: Functor class to compute (1-x) - useful for probablity calculations
// Here not means the log of the probability of not the predicate
// assumes that for integer types the max range value is "1"
// only sensible for real types
class vil_math_not_functor
{
 public:
  float operator()(float x)       const { return x<1.0f?(1.0f-x):0.0f; }
  double operator()(double x)     const { return x<1.0?1.0-x:0.0; }
};

//: Functor class to compute log(1-x) - useful for probablity calculations
// Here not means the log of the probability of not the predicate
// assumes that for integer types the max range value is "1"
// only sensible for real types
class vil_math_log_not_functor
{
 public:
  float operator()(float x)       const { return x<1.0f?vcl_log(1.0f-x):0.0f; }
  double operator()(double x)     const { return x<1.0?vcl_log(1.0-x):0.0; }
};


#endif
