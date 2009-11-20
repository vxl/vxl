// This is brl/bbas/bsta/bsta_beta.h
#ifndef bsta_beta_txx_
#define bsta_beta_txx_

#include "bsta_beta.h"
#include <vnl/vnl_bignum.h>

// Factorial
vnl_bignum factorial(int n)
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

/*template <class T>
void bsta_beta<T>::set_alpha_beta(T alpha, T beta)
{ 
  if (alpha_ = alpha < 0) 
    alpha_=T(0); 
  if (beta_ = beta <0) 
    beta_=T(0); 
}*/

//: pre: x should be in [0,1]
template <class T>
T bsta_beta<T>::prob_density(T x) const 
{ 
  if (x >=T(0) && x<=T(1)) 
    return (vcl_pow(x, alpha_-T(1))*vcl_pow(1-x,beta_-T(1)))/vnl_beta(alpha_,beta_);
  else return T(0); 
}

// cumulative distribution function
template <class T>
T bsta_beta<T>::cum_dist_funct(T x)
{
  unsigned a = static_cast<unsigned>(alpha_);
  unsigned b = static_cast<unsigned>(beta_);
  T Ix=T(0);
  T val;
  for (unsigned j=a; j<=a+b-1; j++) {
    val = factorial(a+b-1)/(factorial(j)*factorial(a+b-1-j));
    val*=vcl_pow(x,j)*vcl_pow(1.0-x, a+b-1-j);
    Ix+=val;
  }
  return Ix;
}

#undef BSTA_BETA_INSTANTIATE
#define BSTA_BETA_INSTANTIATE(T) \
template class bsta_beta<T>

#endif