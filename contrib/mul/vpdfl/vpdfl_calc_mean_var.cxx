// This is mul/vpdfl/vpdfl_calc_mean_var.cxx
#include "vpdfl_calc_mean_var.h"
//:
// \file
// \author Tim Cootes
// \brief Calculate mean and variance of multivariate data.

#include <mbl/mbl_data_array_wrapper.h>

//: Compute mean and variance of data
void vpdfl_calc_mean_var(vnl_vector<double>& mean,
                         vnl_vector<double>& var,
                         const vnl_vector<double>* data, int n)
{
  if (n==0) return;
  int n_dims = data[0].size();
  vnl_vector<double> sum_sq(n_dims);
  mean.set_size(n_dims);
  var.set_size(n_dims);

  double* var_data = var.data_block();
  double* mean_data = mean.data_block();
  double* sum_sq_data = sum_sq.data_block();
  mean.fill(0.0);
  sum_sq.fill(0.0);

  for (int i=0;i<n;i++)
  {
    const double *v = data[i].data_block();
    for (int j=0;j<n_dims;j++)
    {
      mean_data[j] += v[j];
      sum_sq_data[j] += v[j]*v[j];
    }
  }
  mean/=n;

  var.set_size(n_dims);
  for (int j=0;j<n_dims;j++)
  {
    var_data[j] = sum_sq_data[j]/n - mean_data[j]*mean_data[j];
  }
}

//: Compute mean and variance of data
void vpdfl_calc_mean_var(vnl_vector<double>& mean,
                         vnl_vector<double>& var,
                         mbl_data_wrapper<vnl_vector<double> >& data)
{
  if (data.is_class("mbl_data_array_wrapper<T>"))
  {
    // Use more efficient algorithm
    mbl_data_array_wrapper<vnl_vector<double> > array_data
                 = static_cast<mbl_data_array_wrapper<vnl_vector<double> >&>( data);
    vpdfl_calc_mean_var(mean,var,array_data.data(),array_data.size());
    return;
  }

  unsigned long n = data.size();
  if (n==0L) return;
  data.reset();
  int n_dims = data.current().size();
  vnl_vector<double> sum_sq(n_dims);
  mean.set_size(n_dims);
  var.set_size(n_dims);

  double* var_data = var.data_block();
  double* mean_data = mean.data_block();
  double* sum_sq_data = sum_sq.data_block();
  mean.fill(0.0);
  sum_sq.fill(0.0);

  for (unsigned long i=0;i<n;i++)
  {
    const double *v = data.current().data_block();
    for (int j=0;j<n_dims;j++)
    {
      mean_data[j] += v[j];
      sum_sq_data[j] += v[j]*v[j];
    }
    data.next();
  }
  mean/=n;

  var.set_size(n_dims);
  for (int j=0;j<n_dims;j++)
  {
    var_data[j] = sum_sq_data[j]/n - mean_data[j]*mean_data[j];
  }
}
