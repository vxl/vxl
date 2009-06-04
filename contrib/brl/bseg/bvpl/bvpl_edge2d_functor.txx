#ifndef bvpl_edge2d_functor_txx_
#define bvpl_edge2d_functor_txx_

#include "bvpl_edge2d_functor.h"
#include <vcl_cmath.h>

template <class T>
void bvpl_edge2d_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{
  if (d.c_ == 1) {
    P_ += vcl_log(val);
    P_norm += vcl_log(min_P_);
  }
  else if (d.c_ == -1){
    P_ += vcl_log(T(1.0)-val);
    P_norm += vcl_log(T(1.0) - min_P_);
  }
    
  P1_ += vcl_log(val);
  P0_ += vcl_log(T(1.0)-val);
  P05_ += vcl_log(T(0.5));

  
  P1_norm= vcl_log(min_P_);
  P0_norm= vcl_log(T(1.0)-min_P_);
 

}

template <class T>
T bvpl_edge2d_functor<T>::result() 
{ 
  //normalize probabilities w.r.t an empty region
  P_ -= P_norm;
  P1_ -= P_norm;
  P0_ -= P_norm;
  //P05_ -= P_norm;

  //normalize w.r.t other configurations
  T t1 = vcl_exp(P1_ - P_);
  T t2 = vcl_exp(P0_ - P_); 
  T t3 = vcl_exp(P05_ - P_);
  
  return P0_; 
}

#define BVPL_EDGE2D_FUNC_INSTANTIATE(T) \
template class bvpl_edge2d_functor<T>

#endif