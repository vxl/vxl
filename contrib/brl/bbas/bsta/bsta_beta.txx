// This is brl/bbas/bsta/bsta_beta.h
#ifndef bsta_beta_txx_
#define bsta_beta_txx_

#include "bsta_beta.h"

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

#undef BSTA_BETA_INSTANTIATE
#define BSTA_BETA_INSTANTIATE(T) \
template class bsta_beta<T>

#endif