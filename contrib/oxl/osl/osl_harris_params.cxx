#include "osl_harris_params.h"

//:
//  \file

//: Make default osl_harris_params object
osl_harris_params::osl_harris_params()
{
  set_defaults();
}

osl_harris_params::osl_harris_params(int corner_count_max_,
                                     float gauss_sigma_)
{
  set_defaults();
  corner_count_max = corner_count_max_;
  gauss_sigma = gauss_sigma_;
}

osl_harris_params::osl_harris_params(int corner_count_max_,
                                     float gauss_sigma_,
                                     float relative_minimum_,
                                     float scale_factor_)
{
  set_defaults();
  corner_count_max = corner_count_max_;
  gauss_sigma = gauss_sigma_;
  relative_minimum = relative_minimum_;
  scale_factor = scale_factor_;
}

void osl_harris_params::set_defaults()
{
  verbose = true;
  col_start_index = 0;
  row_start_index = 0;
  pab_emulate = false;
  //----------
  corner_count_max = 300;
  gauss_sigma = 0.7f;
  relative_minimum = 0.00001f;
  scale_factor = 0.04f;
  adaptive = true;
  adaptive_window_size = 32;
  density_thresh = 0.5f;
  corner_count_low = 40;
}

void osl_harris_params::set_adaptive(int corner_count_low_,
                                     int adaptive_window_size_,
                                     float density_thresh_)
{
  adaptive = true;
  adaptive_window_size = adaptive_window_size_;
  density_thresh = density_thresh_;
  corner_count_low = corner_count_low_;
}
