// This is contrib/mul/vil3d/vil3d_reflect.h
#ifndef vil3d_flip_h_
#define vil3d_flip_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Kevin de Souza, Ian Scott.
// \brief functions to create a reflected image view


#include <vil3d/vil3d_image_view.h>


//: Create a reflected view in which i -> ni-1-i.
//  i.e. vil3d_reflect_i(view)(i, j, k, p) = view(ni-1-i, j, k, p)
//  O(1).
// \relates vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_reflect_i(const vil3d_image_view<T>& v)
{
  return vil3d_image_view<T>(v.memory_chunk(),
                             v.origin_ptr()+(v.ni()-1)*v.istep(),
                             v.ni(), v.nj(), v.nk(), v.nplanes(),
                             -v.istep(), v.jstep(), v.kstep(), v.planestep());
}


//: Create a reflected view in which j -> nj-1-j.
//  i.e. vil3d_reflect_j(view)(i, j, k, p) = view(i, nj-1-j, k, p)
//  O(1).
// \relates vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_reflect_j(const vil3d_image_view<T>& v)
{
  return vil3d_image_view<T>(v.memory_chunk(),
                             v.origin_ptr()+(v.nj()-1)*v.jstep(),
                             v.ni(), v.nj(), v.nk(), v.nplanes(),
                             v.istep(), -v.jstep(), v.kstep(), v.planestep());
}


//: Create a reflected view in which k -> nk-1-k.
//  i.e. vil3d_reflect_k(view)(i, j, k, p) = view(i, j, nk-1-k, p)
//  O(1).
// \relates vil3d_image_view
template<class T>
inline vil3d_image_view<T> vil3d_reflect_k(const vil3d_image_view<T>& v)
{
  return vil3d_image_view<T>(v.memory_chunk(),
                             v.origin_ptr()+(v.nk()-1)*v.kstep(),
                             v.ni(), v.nj(), v.nk(), v.nplanes(),
                             v.istep(), v.jstep(), -v.kstep(), v.planestep());
}


#endif // vil3d_reflect_h_
