#ifndef pdf1d_calc_mean_var_h
#define pdf1d_calc_mean_var_h
//:
// \file
// \author Tim Cootes
// \brief Calculate mean and variance of 1D data.

#include <vnl/vnl_vector.h>

//: Compute mean and variance of data
void pdf1d_calc_mean_var(double& mean, double& var,
                         const double* data, int n);

//: Compute mean and variance of data
void pdf1d_calc_mean_var(double& mean, double& var,
                         const vnl_vector<double>& d);

#endif // pdf1d_calc_mean_var_h
