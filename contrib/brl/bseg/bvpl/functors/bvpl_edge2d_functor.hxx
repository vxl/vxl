#ifndef bvpl_edge2d_functor_hxx_
#define bvpl_edge2d_functor_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include "bvpl_edge2d_functor.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// Default constructor
template <class T>
bvpl_edge2d_functor<T>::bvpl_edge2d_functor()
{
  this->init();
}

//:Initializes all local variables
template <class T>
void bvpl_edge2d_functor<T>::init()
{
  P_ = T(0);
  P0_ = T(0);
  P1_ = T(0);
  P05_ = T(0);
  P_norm = T(0);
  P0_norm = T(0);
  P1_norm = T(0);
  P05_norm = T(0);
  min_P_ = T(0.01);
  n_=0;
}

template <class T>
void bvpl_edge2d_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{

  if (d.c_ == 1) {
    P_ += std::log(val);
    //P_norm += std::log(min_P_);
  }
  else if (d.c_ == -1){
    P_ += std::log(T(1.0)-val);
    //P_norm += std::log(T(1.0) - min_P_);
  }

  //P1_ += std::log(val);
  //P0_ += std::log(T(1.0)-val);
  //P05_ += std::log(T(0.5));

  //std::cerr << val << " and " << int(d.c_) <<" P " <<P_ << " P0 " <<P0_ << " P1 " <<P1_ << std::endl ;
  //P1_norm += std::log(min_P_);
  //P0_norm += std::log(T(1.0)-min_P_);
  n_++;
}

template <class T>
T bvpl_edge2d_functor<T>::result()
{
#if 0
  //normalize probabilities w.r.t an empty region
  P_ -= P_norm;
  P1_ -= P1_norm;
  P0_ -= P0_norm;
  P05_ -= P_norm;
#endif

  P_/=(T)n_;
  //normalize w.r.t other configurations
  //T t1 = std::exp(P1_ - P_);
  //T t2 = std::exp(P0_ - P_);
  //T t3 = std::exp(P05_ - P_);

  T result = std::exp(P_);

  //reset all variables
  init();

  return result;
}

#define BVPL_EDGE2D_FUNCTOR_INSTANTIATE(T) \
template class bvpl_edge2d_functor<T >

#endif
