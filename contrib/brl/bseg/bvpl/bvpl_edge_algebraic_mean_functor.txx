#ifndef bvpl_edge_algebraic_mean_functor_txx_
#define bvpl_edge_algebraic_mean_functor_txx_
//:
// \file
#include "bvpl_edge_algebraic_mean_functor.h"
#include <vcl_cmath.h>
#include <vcl_iostream.h>


// Default constructor
template <class T>
bvpl_edge_algebraic_mean_functor<T>::bvpl_edge_algebraic_mean_functor()
{
  this->init();
}

//:Initializes all local variables
template <class T>
void bvpl_edge_algebraic_mean_functor<T>::init()
{
  P0_ = T(0);
  P1_ = T(0);
  n0_=0;
  n1_=0;
}

template <class T>
void bvpl_edge_algebraic_mean_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{
  
  if (d.c_ == 1) {
    P1_ += (val);
    n1_++;
  }
  else if (d.c_ == -1){
    P0_ += (T(1.0)-val);
    n0_++;
  }
}

template <class T>
T bvpl_edge_algebraic_mean_functor<T>::result()
{
  
  P0_/=(T)n0_;
  P1_/=(T)n1_;
  
  T result = P0_*P1_;
  
  //reset all variables
  init();
  
  return result;
}

#define BVPL_EDGE_ALGEBRAIC_MEAN_FUNCTOR_INSTANTIATE(T) \
template class bvpl_edge_algebraic_mean_functor<T >

#endif
