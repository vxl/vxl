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
T bsta_parzen_sphere<T,n>::prob_density(const vnl_vector_fixed<T, n>& pt) const
{
  unsigned size = samples_.size();
  if(!size) return T(0);
  T nsamp = static_cast<T>(size);
  T density = static_cast<T>(0);
  vector_ null;
  null.fill(T(0));
  T var = bandwidth_*bandwidth_;
  bsta_gaussian_sphere<T,n> gs(null, var);
  for(sv_const_it sit = samples_.begin();
      sit != samples_.end(); ++sit)
    {
      vector_ s = *sit;
      gs.set_mean(s);
      density += gs.prob_density(pt);
    }
  density/=nsamp;
  return density;
}

//: The probability density integrated over a box (returns a probability)
template <class T, unsigned int n>
T bsta_parzen_sphere<T,n>::probability(const vnl_vector_fixed<T, n>& min_pt,
                                       const vnl_vector_fixed<T, n>& max_pt) const
{
  unsigned size = samples_.size();
  if(!size) return static_cast<T>(0);
  T nsamp = static_cast<T>(size);
  T prob = static_cast<T>(0);
  vector_ null;
  null.fill(T(0));
  T var = bandwidth_*bandwidth_;
  bsta_gaussian_sphere<T,n> gs(null, var);
  for(sv_const_it sit = samples_.begin();
      sit != samples_.end(); ++sit)
    {
      vector_ s = *sit;
      gs.set_mean(s);
      prob += gs.probability(min_pt, max_pt);
    }
  prob/=nsamp;
  return prob;
}

#define BSTA_PARZEN_SPHERE_INSTANTIATE(T,n) \
template class bsta_parzen_sphere<T,n >


#endif // bsta_parzen_sphere_txx_
