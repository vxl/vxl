#include "boxm2_vecf_inverse_square_weighting_function.h"

boxm2_vecf_inverse_square_weighting_function::
boxm2_vecf_inverse_square_weighting_function(double max_val, double x_at_10_percent_max)
{
  inv_max_val_ = 1.0/max_val;
  // set a such that f(x_10_percent_max) = 0.1 * max_val
  a_ = 9.0 / (max_val * x_at_10_percent_max * x_at_10_percent_max);
}
