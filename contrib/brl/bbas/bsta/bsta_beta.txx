// This is brl/bbas/bsta/bsta_beta.txx
#ifndef bsta_beta_txx_
#define bsta_beta_txx_
//:
// \file
#include "bsta_beta.h"
#include <vnl/vnl_bignum.h>

// Factorial
static inline vnl_bignum factorial(int n)
{
  if (n <= 1)
    return vnl_bignum(1);
  else
    return n * factorial(n-1);
}


//: constructs from a set of sample values
template <class T>
bsta_beta<T>::bsta_beta(vcl_vector<T> x)
{
  T mean=0;
  T var=0;

  for (unsigned i=0; i<x.size(); i++) {
    mean+=x[i];
  }

  mean/=x.size();

  for (unsigned i=0; i<x.size(); i++) {
    T diff = x[i]-mean;
    var+=diff*diff;
  }

  var/=x.size();

  T t = (mean*(1-mean)/var)-1;
  alpha_=mean*t;
  beta_=(1-mean)*t;
}

template <class T>
bool bsta_beta<T>::bsta_beta_from_moments(T mean, T var, T& alpha, T& beta)
{
  if (var == 0)
    return false;

  T t = mean*(1-mean)/var-1;
  alpha=mean*t;
  beta=(1-mean)*t;
  return true;
}

//: pre: x should be in [0,1]
template <class T>
T bsta_beta<T>::prob_density(T x) const
{
  if (x<T(0)||x>T(1))
      return 0;
  else if (x == 0 && alpha_<T(1)) {
      return T(1e10);
  }
  else if (x == 1 && beta_<T(1)) {
      return T(1e10);
  }
  else
  {
    T a = (T)vnl_log_beta(alpha_,beta_);

    T b = (alpha_-1)*vcl_log(x) ;
    T c = (beta_-1)*vcl_log(1-x) ;

    T ans=vcl_exp(b+c-a);
    return ans;
  }
}

// cumulative distribution function
template <class T>
T bsta_beta<T>::cum_dist_funct(T x) const
{
  unsigned a = static_cast<unsigned>(alpha_);
  unsigned b = static_cast<unsigned>(beta_);
  T Ix=T(0);
  T val;
  for (unsigned j=a; j<=a+b-1; j++) {
    val = factorial(a+b-1)/(factorial(j)*factorial(a+b-1-j));
    val *= vcl_pow(x,T(j))*vcl_pow(1-x, T(a+b-1-j));
    Ix+=val;
  }
  return Ix;
}

#undef BSTA_BETA_INSTANTIATE
#define BSTA_BETA_INSTANTIATE(T) \
template class bsta_beta<T >

#endif
