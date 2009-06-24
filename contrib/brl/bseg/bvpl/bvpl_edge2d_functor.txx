#ifndef bvpl_edge2d_functor_txx_
#define bvpl_edge2d_functor_txx_
//:
// \file
#include "bvpl_edge2d_functor.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>


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
    P_ += vcl_log(val);
    //P_norm += vcl_log(min_P_);
  }
  else if (d.c_ == -1){
    P_ += vcl_log(T(1.0)-val);
    //P_norm += vcl_log(T(1.0) - min_P_);
  }
  
  //P1_ += vcl_log(val);
  //P0_ += vcl_log(T(1.0)-val);
  //P05_ += vcl_log(T(0.5));
  
  //vcl_cerr << val << " and " << int(d.c_) <<" P " <<P_ << " P0 " <<P0_ << " P1 " <<P1_ << vcl_endl ;
  //P1_norm += vcl_log(min_P_);
  //P0_norm += vcl_log(T(1.0)-min_P_);
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
  //T t1 = vcl_exp(P1_ - P_);
  //T t2 = vcl_exp(P0_ - P_);
  //T t3 = vcl_exp(P05_ - P_);
  
  T result = vcl_exp(P_);
  
  //reset all variables
  init();
  
  return result;
}

#define BVPL_EDGE2D_FUNCTOR_INSTANTIATE(T) \
template class bvpl_edge2d_functor<T >

#endif
