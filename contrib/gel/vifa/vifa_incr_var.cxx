// This is gel/vifa/vifa_incr_var.cxx

#include "vifa_incr_var.h"

void vifa_incr_var::
add_sample(double  data_point,
           double  prev_factor,
           double  curr_factor)
{
  // Update the mean
  double  old_value = data_mean_;
  data_mean_ = (prev_factor * data_mean_) + (curr_factor * data_point);

  // Update the variance
  double  old_delta = data_mean_ - old_value;
  double  data_delta = data_mean_ - data_point;
  data_var_ = (prev_factor * (data_var_ + old_delta * old_delta)) +
        (curr_factor * data_delta * data_delta);

  // Track min/max data points
  min_max_.update(data_point);

  // Increment the count
  n_++;
}

void vifa_incr_var::
add_sample(double  data_point)
{
  int    current_n = this->get_n();
  double  prev_factor = ((double)current_n) / (current_n + 1);
  double  curr_factor = 1.0 / (current_n + 1);
  add_sample(data_point, prev_factor, curr_factor);
}
