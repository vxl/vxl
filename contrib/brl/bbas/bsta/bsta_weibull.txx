// This is brl/bbas/bsta/bsta_weibull.txx
#ifndef bsta_weibull_txx_
#define bsta_weibull_txx_
//:
// \file

#include "bsta_weibull.h"
#include <vcl_cassert.h>
#include <vcl_cmath.h>


template <class T>
bsta_weibull<T>::bsta_weibull():  lambda_(static_cast<vector_>(1)),
                                         mu_(static_cast<vector_>(0)),
                                         k_(static_cast<vector_>(1))
{}

template <class T>
bsta_weibull<T>::bsta_weibull(vector_ const& lambda, vector_ const& k):
  lambda_(lambda), mu_(static_cast<vector_>(0)), k_(k)
{}

template <class T>
bsta_weibull<T>::bsta_weibull(vector_ const& lambda, vector_ const& k,
                              vector_ const& mu)
: lambda_(lambda), mu_(mu), k_(k)
{}

template <class T>
T bsta_weibull<T>::prob_density(const vector_& pt) const
{
  double dk = static_cast<double>(k_);
  assert(dk>0);
  double la = static_cast<double>(lambda_);
  assert(la>0);
  double m = static_cast<double>(mu_);
  double c = dk/la;
  double x = static_cast<double>(pt)-m;
  x /= la;
  c *= vcl_pow(x,dk-1.0);
  c *= vcl_exp(-vcl_pow(x,dk));
  return static_cast<T>(c);
}

template <class T>
T bsta_weibull<T>::probability(const vector_& min_pt,
                               const vector_& max_pt) const
{
  double dk = static_cast<double>(k_);
  assert(dk>0);
  double la = static_cast<double>(lambda_);
  assert(la>0);
  double m = static_cast<double>(mu_);
  double xmin = min_pt-m;
  double xmax = max_pt-m;
  double tp = vcl_exp(-vcl_pow(xmin/la, dk));
  double tm = vcl_exp(-vcl_pow(xmax/la, dk));
  return static_cast<T>(tp-tm);
}


// only allow instantiation of univariate weibull distributions for now
#define BSTA_WEIBULL_INSTANTIATE(T) \
template class bsta_weibull<T >


#endif // bsta_weibull_txx_
