// This is brl/bbas/bsta/bsta_parzen_sphere.txx
#ifndef bsta_parzen_sphere_txx_
#define bsta_parzen_sphere_txx_
//:
// \file

#include "bsta_parzen_sphere.h"
#include "bsta_gaussian_sphere.h"
#include <vcl_cassert.h>


//: The probability density at sample pt
template <class T, unsigned int n>
T bsta_parzen_sphere<T,n>::prob_density(typename bsta_parzen<T,n>::vector_ const& pt) const
{
  unsigned size = bsta_parzen<T,n>::samples_.size();
  if (!size) return T(0);
  T nsamp = static_cast<T>(size);
  T density = static_cast<T>(0);
  typename bsta_parzen<T,n>::vector_ null(T(0));// works even if vector_ is equivalent to T, i.e. n =1
  T var = bandwidth_*bandwidth_;
  bsta_gaussian_sphere<T,n> gs(null, var);
  for (typename bsta_parzen<T,n>::sv_const_it sit = bsta_parzen<T,n>::samples_.begin();
       sit != bsta_parzen<T,n>::samples_.end(); ++sit)
  {
    typename bsta_parzen<T,n>::vector_ s = *sit;
    gs.set_mean(s);
    density += gs.prob_density(pt);
  }
  density/=nsamp;
  return density;
}

//: The probability density integrated over a box (returns a probability)
template <class T, unsigned int n>
T bsta_parzen_sphere<T,n>::probability(typename bsta_parzen<T,n>::vector_ const& min_pt,
                                       typename bsta_parzen<T,n>::vector_ const& max_pt) const
{
  unsigned size = bsta_parzen<T,n>::samples_.size();
  if (!size) return static_cast<T>(0);
  T nsamp = static_cast<T>(size);
  T prob = static_cast<T>(0);
  typename bsta_parzen<T,n>::vector_ null(T(0));// works even if vector_ is equivalent to T, i.e. n =1
  T var = bandwidth_*bandwidth_;
  bsta_gaussian_sphere<T,n> gs(null, var);
  for (typename bsta_parzen<T,n>::sv_const_it sit = bsta_parzen<T,n>::samples_.begin();
       sit != bsta_parzen<T,n>::samples_.end(); ++sit)
  {
    typename bsta_parzen<T,n>::vector_ s = *sit;
    gs.set_mean(s);
    prob += gs.probability(min_pt, max_pt);
  }
  prob/=nsamp;
  return prob;
}

#define BSTA_PARZEN_SPHERE_INSTANTIATE(T,n) \
template class bsta_parzen_sphere<T,n >


#endif // bsta_parzen_sphere_txx_
