// This is mul/pdf1d/pdf1d_prob_ks.cxx
#include <iostream>
#include <cmath>
#include "pdf1d_prob_ks.h"
//:
// \file
// \author Tim Cootes
// \brief Probability used in Kolmogorov-Smirnov test

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

constexpr double f1 = 0.001;
const double f2 = 1.0e-8;

//: Probability used in Kolmogorov-Smirnov test
//  This is the prob. that the maximum difference between two cumulative
//  distributions is larger than x.
double pdf1d_prob_ks(double x)
{
  double k=2.0,sum=0.0,term,previous_term=0.0;

  double a2 = -2.0*x*x;
  for (int j=1;j<=100;j++)
  {
    term=k*std::exp(a2*j*j);
    sum += term;
    if (std::fabs(term) <= f1*previous_term || std::fabs(term) <= f2*sum) return sum;
    k = -k;
    previous_term=std::fabs(term);
  }
  return 1.0;
}
