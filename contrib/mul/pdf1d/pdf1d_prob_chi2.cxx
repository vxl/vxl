// This is mul/pdf1d/pdf1d_prob_chi2.cxx
#include <iostream>
#include <cstdlib>
#include "pdf1d_prob_chi2.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_gamma.h>

double pdf1d_chi2_for_cum_prob(double p, int n_dof, double tol)
{
  if ((p<0) | (p>=1.0))
  {
    std::cerr<<"pdf1d_chi2_for_cum_prob : Illegal value for probability. (Outside range [0,1) )"<<std::endl;
    std::abort();
  }

  if (p==0) return 0;

  double d_step = n_dof; // prob = 50% ish
  double low_chi = 0;
  double high_chi = d_step;

  //double p_low = 0; // not used
  double p_high = pdf1d_cum_prob_chi2(n_dof,high_chi);

  // First step along till p_high >= p
  while (p_high<p)
  {
    low_chi = high_chi;
    // p_low = p_high; // not used
    high_chi += d_step;
    p_high = pdf1d_cum_prob_chi2(n_dof,high_chi);
  }

  // p_low and p_high now straddle answer
  double mid_chi = 0.5 * (low_chi+high_chi);
  double p_mid;

  while ((mid_chi-low_chi)>tol)
  {
    p_mid = pdf1d_cum_prob_chi2(n_dof,mid_chi);
    if (p_mid>p)
    {
      // Use low & mid as limits
      high_chi = mid_chi;
    }
    else
    {
      // Use mid and high as limits
      low_chi = mid_chi;
    }

    mid_chi = 0.5 * (low_chi+high_chi);
  }

  return mid_chi;
}


double pdf1d_cum_prob_chi2(int n_dof, double chi2)
{
  return mbl_gamma_p((double) n_dof/2.0 , chi2/2.0 );
}
