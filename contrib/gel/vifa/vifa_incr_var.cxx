// This is contrib/gel/vifa/vifa_incr_var.c

#include <vifa/vifa_incr_var.h>

vifa_incr_var::
vifa_incr_var(void) :
	data_mean_(0.0),
	data_var_(0.0),
	n_(0),
	min_(1e6),
	max_(1e-6)
{
	// No further explicit initialization needed
}

double vifa_incr_var::
get_mean(void) const
{
	return data_mean_;
}

double vifa_incr_var::
get_var(void) const
{
	return data_var_;
}

int vifa_incr_var::
get_n(void) const
{
	return n_;
}

double vifa_incr_var::
get_min(void) const
{
	return min_;
}

double vifa_incr_var::
get_max(void) const
{
	return max_;
}

void vifa_incr_var::
add_sample(double	data_point,
		   double	prev_factor,
		   double	curr_factor)
{
	// Update the mean
	double	old_value = data_mean_;
	data_mean_ = (prev_factor * data_mean_) + (curr_factor * data_point);

	// Update the variance
	double	old_delta = data_mean_ - old_value;
	double	data_delta = data_mean_ - data_point;
	data_var_ = (prev_factor * (data_var_ + old_delta * old_delta)) +
				(curr_factor * data_delta * data_delta);

	// Track min/max data points
	if (data_point < min_)
	{
		min_ = data_point;
	}
	if (data_point > max_)
	{
		max_ = data_point;
	}

	// Increment the count
	n_++;
}

void vifa_incr_var::
add_sample(double	data_point) 
{
	int		current_n = this->get_n();
	double	prev_factor = ((double)current_n) / (current_n + 1);
	double	curr_factor = 1.0 / (current_n + 1);
	add_sample(data_point, prev_factor, curr_factor);
}

