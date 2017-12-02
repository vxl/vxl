#ifndef mbl_jarque_bera_h_
#define mbl_jarque_bera_h_

#include <vnl/vnl_gamma.h>
//:
// \file
// \author Ian Scott
// \date 20-Aug-2003
// \brief Jarque Bera test for normality


//: Jarque Bera test for normality
// Returns the p-value - the probability that
// the data is normal. If you want a confidence alpha of 0.05
// then the data is normal if (mbl_jarque_bera(start,end) > 0.05)
//
// Note that the test is not particular valuable for small data set.
// For more info, see J. B. Cromwell, W. C. Labys and M. Terraza (1994):
// Univariate Tests for Time Series Models, Sage, California, USA, pages 20-22.
template <class InIt>
double mbl_jarque_bera(InIt start, InIt finish)
{
  unsigned count = 0;
  double mean = 0.0;
  double stddev = 0.0;
  for (InIt it=start; it!=finish; ++it)
  {
    ++count;
    const double x = *it;
    mean += x;
    stddev += x*x;
  }

  const double n = count;
  mean /= n;
  stddev = std::sqrt((stddev - mean*mean*n)/(n-1));


  double S=0.0, K=0.0;
  for (InIt it=start; it!=finish; ++it)
  {
    const double x = ((*it) - mean)/stddev;
    const double x_cubed = x*x*x;
    S += x_cubed;
    K += x_cubed * x;
  }
  S /= n;
  K = K/n - 3;

  const double jbstat = n * ( (S*S)/6  +  (K*K)/24 );

  return 1 - vnl_cum_prob_chi2(2, jbstat);
}


#endif // mbl_jarque_bera_h_
