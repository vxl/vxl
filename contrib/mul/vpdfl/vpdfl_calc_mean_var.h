#ifndef vpdfl_calc_mean_var_h
#define vpdfl_calc_mean_var_h
//:
// \file
// \author Tim Cootes
// \brief Calculate mean and variance of multivariate data.

#include <vnl/vnl_vector.h>

//: Compute mean and variance of data
void vpdfl_calc_mean_var(vnl_vector<double>& mean,
                         vnl_vector<double>& var,
                         const vnl_vector<double>* data, int n);


#endif // vpdfl_calc_mean_var_h

