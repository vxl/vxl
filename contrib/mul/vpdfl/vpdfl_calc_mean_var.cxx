//:
// \file
// \author Tim Cootes
// \brief Calculate mean and variance of multivariate data.

#include <vpdfl/vpdfl_calc_mean_var.h>

//: Compute mean and variance of data
void vpdfl_calc_mean_var(vnl_vector<double>& mean,
                         vnl_vector<double>& var,
                         const vnl_vector<double>* data, int n)
{
  if (n==0) return;
  int n_dims = data[0].size();
  vnl_vector<double> sum_sq(n_dims);
  mean.resize(n_dims);
  var.resize(n_dims);

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

	var.resize(n_dims);
  for (int j=0;j<n_dims;j++)
  {
    var_data[j] = sum_sq_data[j]/n - mean_data[j]*mean_data[j];
  }
}
