// This is brl/bbas/bsta/bsta_parzen_sphere.txx
#ifndef bsta_parzen_sphere_txx_
#define bsta_parzen_sphere_txx_
//:
// \file

#include "bsta_parzen_sphere.h"
//
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h> // needed for bsta_parzen_sphere<T,n>::covar_type
#include <vnl/vnl_numeric_traits.h>
#include "bsta_gaussian_sphere.h"
#include <vcl_cassert.h>

//helper classes to support partial template instantiation on dimension
//these functions work for any dimension
namespace
{
  template <class T, unsigned int n>
  typename bsta_distribution<T,n>::vector_type
  compute_mean(vcl_vector<typename bsta_distribution<T,n>::vector_type > const& samples)
  {
    typedef typename bsta_distribution<T,n>::vector_type vect_t;
    typedef typename vcl_vector<vect_t >::const_iterator sit_t;
    vect_t sum(T(0));
    sit_t sit = samples.begin();
    T nsamp = static_cast<T>(samples.size());
    for (; sit != samples.end(); ++sit)
      sum += (*sit);
    sum /= nsamp;
    return sum;
  }


  template <class T , unsigned int n >
  T compute_prob_density(typename bsta_distribution<T,n>::vector_type const& pt,
                         vcl_vector<typename bsta_distribution<T,n>::vector_type > const& samples, T var)
  {
    typedef typename bsta_distribution<T,n>::vector_type vect_t;
    typedef typename vcl_vector<vect_t >::const_iterator sit_t;
    unsigned size = samples.size();
    if (!size) return T(0);
    T nsamp = static_cast<T>(size);
    T density = static_cast<T>(0);
    vect_t null(T(0)); // works even if vector_ is equivalent to T, i.e. n =1
    bsta_gaussian_sphere<T,n> gs(null, var);
    sit_t sit = samples.begin();
    for (; sit != samples.end(); ++sit)
    {
      vect_t s = *sit;
      gs.set_mean(s);
      density += gs.prob_density(pt);
    }
    density/=nsamp;
    return density;
  }

  template <class T, unsigned int n>
  T compute_probability(typename bsta_distribution<T,n>::vector_type const& min_pt, typename bsta_distribution<T,n>::vector_type const& max_pt, vcl_vector<typename bsta_distribution<T,n>::vector_type > const& samples, T var)
  {
    typedef typename bsta_distribution<T,n>::vector_type vect_t;
    typedef typename vcl_vector<vect_t >::const_iterator sit_t;
    unsigned size = samples.size();
    if (!size) return static_cast<T>(0);
    T nsamp = static_cast<T>(size);
    T prob = static_cast<T>(0);
    vect_t null(T(0)); // works even if vector_ is equivalent to T, i.e. n =1
    bsta_gaussian_sphere<T,n> gs(null, var);
    sit_t sit = samples.begin();
    for (; sit != samples.end(); ++sit)
    {
      vect_t s = *sit;
      gs.set_mean(s);
      prob += gs.probability(min_pt, max_pt);
    }
    prob/=nsamp;
    return prob;
  }
}; // end namespace

// general case
template <class T, unsigned int n>
vnl_vector_fixed<T,n> bsta_parzen_sphere<T,n>::mean() const
{
  return compute_mean<T,n>(bsta_parzen<T,n>::samples_);
}

// for scalar samples
template <class T >
T bsta_parzen_sphere<T,1>::mean() const
{
  return compute_mean<T,1>(bsta_parzen<T,1>::samples_);
}

//: the covariance matrix for a Parzen distribution, general case
template <class T, unsigned int n>
typename bsta_parzen_sphere<T,n>::covar_type bsta_parzen_sphere<T,n>::covar() const
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;
  typedef typename bsta_parzen<T,n>::sv_const_it sit_t;
  typedef typename bsta_parzen_sphere<T,n>::covar_type mat_t;

  unsigned size = bsta_parzen<T,n>::samples_.size();
  if (!size)
    return mat_t(T(0));
  // compute the sample covariance matrix
  mat_t scovar; // fill with zeros
  scovar.fill(T(0));
  for (sit_t sit = bsta_parzen<T,n>::samples_.begin();
       sit != bsta_parzen<T,n>::samples_.end(); ++sit)
  {
    vect_t s = *sit;
    for (unsigned r = 0; r<n; ++r){
      if (n>1)
        for (unsigned c = r; c<n; ++c)
          scovar[r][c] += s[r]*s[c];
      for (unsigned c = 0; c<r; ++c)
        scovar[r][c] = scovar[c][r];
    }
  }
  scovar/=static_cast<T>(size);
  vect_t mu = this->mean();
  for (unsigned r = 0; r<n; ++r) {
    if (n>1)
      for (unsigned c = r; c<n; ++c) {
        scovar[r][c] -= mu[r]*mu[c];
        if (r==c)
          scovar[r][c] += bandwidth_*bandwidth_;
      }
    for (unsigned c = 0; c<r; ++c)
      scovar[r][c] = scovar[c][r];
  }
  return scovar;
}

//: the covariance matrix for a Parzen distribution, specialization for 1-d case
template <class T >
T bsta_parzen_sphere<T,1>::covar() const
{
  typedef typename bsta_parzen<T,1>::vect_t v_type;
  typedef typename bsta_parzen<T,1>::sv_const_it sit_t;
  unsigned size = bsta_parzen<T,1>::samples_.size();
  if (!size)
    return T(0);
  // compute the sample covariance matrix
  T scovar(0);
  for (sit_t sit = bsta_parzen<T,1>::samples_.begin();
       sit != bsta_parzen<T,1>::samples_.end(); ++sit)
  {
    v_type s = *sit;
    scovar += s*s;
  }
  scovar/=static_cast<T>(size);
  v_type mu = this->mean();
  scovar -= mu*mu;
  scovar += bandwidth_*bandwidth_;
  return scovar;
}

//: The probability density at sample pt, the general case
template <class T, unsigned int n>
T bsta_parzen_sphere<T,n>::prob_density(typename bsta_distribution<T,n>::vector_type const& pt) const
{
  return compute_prob_density<T,n>(pt, bsta_parzen<T,n>::samples_,
                                   bandwidth_*bandwidth_);
}

//: The probability density at sample pt, specialized to scalar samples
template <class T >
T bsta_parzen_sphere<T,1>::
prob_density(typename bsta_distribution<T,1>::vector_type const& pt) const
{
  return compute_prob_density<T,1>(pt, bsta_parzen<T,1>::samples_,
                                   bandwidth_*bandwidth_);
}

//: The probability density integrated over a box, general case
// \returns a probability
template <class T, unsigned int n>
T bsta_parzen_sphere<T,n>::probability(typename bsta_distribution<T,n>::vector_type const& min_pt,
                                       typename bsta_distribution<T,n>::vector_type const& max_pt) const
{
  return compute_probability<T,n>(min_pt, max_pt, bsta_parzen<T,n>::samples_,
                                  bandwidth_*bandwidth_);
}

//: The probability density integrated over a box, specialized to scalar case
// \returns a probability
template <class T >
T bsta_parzen_sphere<T,1>::
probability(typename bsta_distribution<T,1>::vector_type const& min_pt,
            typename bsta_distribution<T,1>::vector_type const& max_pt) const
{
  return compute_probability<T,1>(min_pt, max_pt, bsta_parzen<T,1>::samples_,
                                  bandwidth_*bandwidth_);
}

//: general case for distance and idex of nearest sample
template <class T, unsigned int n>
T bsta_parzen_sphere<T,n>::nearest_sample(const typename bsta_distribution<T,n>::vector_type& pt, unsigned & index) const
{
  typedef typename bsta_distribution<T,n>::vector_type vect_t;
  typedef typename bsta_parzen<T,n>::sv_const_it sit_t;
  T min_dist = vnl_numeric_traits<T>::maxval;
  unsigned count = 0; index = 0;
  for (sit_t sit = bsta_parzen<T,n>::samples_.begin();
       sit != bsta_parzen<T,n>::samples_.end(); ++sit, ++count)
  {
    vect_t s = *sit;
    vect_t dif = s-pt;
    T d = dif.magnitude();
    if (d<min_dist){
      index = count;
      min_dist = d;
    }
  }
  return min_dist;
}

//: distance to nearest sample, specialized to scalar case
template <class T >
T bsta_parzen_sphere<T,1>::nearest_sample(const typename bsta_distribution<T,1>::vector_type & pt, unsigned& index) const
{
  typedef typename bsta_distribution<T,1>::vector_type vect_t;
  typedef typename bsta_parzen<T,1>::sv_const_it sit_t;
  T min_dist = vnl_numeric_traits<T>::maxval;
  unsigned count = 0; index = 0;
  for (sit_t sit = bsta_parzen<T,1>::samples_.begin();
       sit != bsta_parzen<T,1>::samples_.end(); ++sit, ++count)
  {
    vect_t s = *sit;
    vect_t dif = s-pt;
    T d = dif;
    if (d<0) d=-d;
    if (d<min_dist){
      index = count;
      min_dist = d;
    }
  }
  return min_dist;
}

#define BSTA_PARZEN_SPHERE_INSTANTIATE(T,n) \
template class bsta_parzen_sphere<T,n >

#endif // bsta_parzen_sphere_txx_
