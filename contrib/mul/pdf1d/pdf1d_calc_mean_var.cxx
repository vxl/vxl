// This is mul/pdf1d/pdf1d_calc_mean_var.cxx
#include "pdf1d_calc_mean_var.h"
//:
// \file
// \author Tim Cootes
// \brief Calculate mean and variance of 1D data.

void pdf1d_calc_mean_var(double& mean, double& var,
                         const vnl_vector<double>& d)
{
  pdf1d_calc_mean_var(mean,var,d.data_block(),d.size());
}
void pdf1d_calc_mean_var(double& mean, double& var,
                         const double* d, int n)
{
  double sum=0;
  double sum2 = 0;
  for (int i=0;i<n;++i)
  {
    sum+=d[i];
    sum2+=d[i]*d[i];
  }

  mean = sum/n;
  var  = (sum2 - n*mean*mean)/(n-1);
}
