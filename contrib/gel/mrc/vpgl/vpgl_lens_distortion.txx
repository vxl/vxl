// This is gel/mrc/vpgl/vpgl_lens_distortion.txx
#ifndef vpgl_lens_distortion_txx_
#define vpgl_lens_distortion_txx_

//:
// \file

#include "vpgl_lens_distortion.h"
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_2d.h>


//: Return the original point that was distorted to this location (inverse of distort)
// \param init is an initial guess at the solution for the iterative solver
// if \p init is NULL then \p point is used as the initial guess
template <class T>
vgl_homg_point_2d<T> 
vpgl_lens_distortion<T>::undistort( const vgl_homg_point_2d<T>& point, 
                                     const vgl_homg_point_2d<T>* init ) const
{
  vgl_point_2d<T> pt = point; 
  vgl_homg_point_2d<T> result = point;
  if(init)
    result = *init;
    
  vgl_vector_2d<T> e = pt - vgl_point_2d<T>(distort(result));
  //: FIXME Do some iteration here to solve for result 
  assert(false);
  
  return result;
}



// Code for easy instantiation.
#undef vpgl_LENS_DISTORTION_INSTANTIATE
#define vpgl_LENS_DISTORTION_INSTANTIATE(T) \
template class vpgl_lens_distortion<T>


#endif // vpgl_lens_distortion_txx_

