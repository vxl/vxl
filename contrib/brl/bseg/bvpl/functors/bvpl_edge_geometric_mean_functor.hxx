#ifndef bvpl_edge_geometric_mean_functor_hxx_
#define bvpl_edge_geometric_mean_functor_hxx_
//:
// \file
#include <iostream>
#include <cmath>
#include "bvpl_edge_geometric_mean_functor.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


// Default constructor
template <class T>
bvpl_edge_geometric_mean_functor<T>::bvpl_edge_geometric_mean_functor()
{
  this->init();
}

//:Initializes all local variables
template <class T>
void bvpl_edge_geometric_mean_functor<T>::init()
{
  P0_ = T(0);
  P1_ = T(0);
  n0_=0;
  n1_=0;
}

template <class T>
void bvpl_edge_geometric_mean_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{

  if (d.c_ > 0) {
    P1_ += std::log(val);
    n1_++;
  }
  else if (d.c_ < 0 ){
    P0_ += std::log(T(1.0)-val);
    n0_++;
  }
}

template <class T>
T bvpl_edge_geometric_mean_functor<T>::result()
{
  //avoid division by zero
  if(!((n0_ | 0 ) | (n1_ | 0)))
  {
    init();
    return (T)0;
  }

  P0_/=(T)n0_;
  P1_/=(T)n1_;

  T result = std::exp(P0_+P1_);

  //reset all variables
  init();

  return result;
}

#define BVPL_EDGE_GEOMETRIC_MEAN_FUNCTOR_INSTANTIATE(T) \
template class bvpl_edge_geometric_mean_functor<T >

#endif
