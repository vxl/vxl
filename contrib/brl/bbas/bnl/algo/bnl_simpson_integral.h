#ifndef BNL_SIMPSON_INTEGRAL_H_
#define BNL_SIMPSON_INTEGRAL_H_
//:
// \file
// \author Kongbin Kang at Brown
// \date   Jan. 17th, 2005
//
#include <bnl/bnl_definite_integral.h>

class bnl_simpson_integral : public bnl_definite_integral
{
 private:
  //: used to wrap the function class to an ordinary function.
  static double int_fnct_(float* x);

 public:

  bnl_simpson_integral() {}

  //: a and b are integral limits respectively.
  // n is the number of intervals used in integral.
  // The actual subinterval used is 2* num_intervals_
  double integral(bnl_integrant_fnct *f, float a, float b, int n);
};

#endif
