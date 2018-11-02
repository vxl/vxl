// This is brl/bbas/bsta/bsta_weibull.hxx
#ifndef bsta_weibull_hxx_
#define bsta_weibull_hxx_
//:
// \file

#include <iostream>
#include <cmath>
#include "bsta_weibull.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


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
  c *= std::pow(x,dk-1.0);
  c *= std::exp(-std::pow(x,dk));
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
  double tp = std::exp(-std::pow(xmin/la, dk));
  double tm = std::exp(-std::pow(xmax/la, dk));
  return static_cast<T>(tp-tm);
}


// only allow instantiation of univariate weibull distributions for now
#define BSTA_WEIBULL_INSTANTIATE(T) \
template class bsta_weibull<T >


#endif // bsta_weibull_hxx_
