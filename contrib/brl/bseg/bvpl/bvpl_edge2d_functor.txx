#ifndef bvpl_edge2d_functor_txx_
#define bvpl_edge2d_functor_txx_

#include "bvpl_edge2d_functor.h"
#include <vcl_cmath.h>

template <class T>
void bvpl_edge2d_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{
  if (d.c_ == 1) 
    P_ += vcl_log(val);
  else if (d.c_ == -1)
    P_ += vcl_log(1.0-val);
    
  P1_ += vcl_log(val);
  P0_ += vcl_log(1.0-val);
  P05_ += vcl_log(0.5);
}

template <class T>
T bvpl_edge2d_functor<T>::result() 
{ 
  return P0_; 
}

#define BVPL_EDGE2D_FUNC_INSTANTIATE(T) \
template class bvpl_edge2d_functor<T>

#endif