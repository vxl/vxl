#ifndef VNL_SIMPSON_INTEGRAL_H_
#define VNL_SIMPSON_INTEGRAL_H_
//:
// \file
// \author Kongbin Kang at Brown
// \date   Jan. 17th, 2005
//
#include <vnl/vnl_definite_integral.h>

class vnl_simpson_integral : public vnl_definite_integral
{
 private:
  //: used to wrap the function class to an ordinary function.
  static double int_fnct_(float* x);

 public:

  vnl_simpson_integral() {}

  //: a and b are integral limits respectively.
  // n is the number of intervals used in integral.
  // The actual subinterval used is 2* num_intervals_
  double integral(vnl_integrant_fnct *f, float a, float b, int n);
};

#endif
