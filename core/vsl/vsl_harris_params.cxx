#include "vsl_harris_params.h"

// -- Make default vsl_harris_params object
vsl_harris_params::vsl_harris_params() 
{
  set_defaults();
}

vsl_harris_params::vsl_harris_params(int corner_count_max_, 
				     float gauss_sigma_)
{
  set_defaults();
  corner_count_max = corner_count_max_;
  gauss_sigma = gauss_sigma_;
}

vsl_harris_params::vsl_harris_params(int corner_count_max_,
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

void vsl_harris_params::set_defaults()
{
  verbose = true;
  col_start_index = 0;
  row_start_index = 0;
  pab_emulate = false;
  //----------
  corner_count_max = 300;
  gauss_sigma = 0.7;
  relative_minimum = 0.00001;
  scale_factor = 0.04;
  adaptive = true;
  adaptive_window_size = 32;
  density_thresh = 0.5;
  corner_count_low = 40;
}

void vsl_harris_params::set_adaptive(int corner_count_low_,
				     int adaptive_window_size_,
				     float density_thresh_) 
{
  adaptive = true;
  adaptive_window_size = adaptive_window_size_;
  density_thresh = density_thresh_;
  corner_count_low = corner_count_low_;
}
