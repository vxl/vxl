#ifndef bvpl_edge2d_functor_txx_
#define bvpl_edge2d_functor_txx_

#include "bvpl_edge2d_functor.h"

template <class T>
bvpl_edge2d_functor<T>::bvpl_edge2d_functor() 
: result_(0)
{
}

template <class T>
bvpl_edge2d_functor<T>::~bvpl_edge2d_functor()
{
}

template <class T>
void bvpl_edge2d_functor<T>::apply(T& val, bvpl_kernel_dispatch& d)
{
  if (d.c_ == '+')
    result_ *= val;
  else if (d.c_ == '-')
    result_ *= (1.0 - val);
}

#define BVPL_EDGE2D_FUNC_INSTANTIATE(T) \
template class bvpl_edge2d_functor<T>;

#endif