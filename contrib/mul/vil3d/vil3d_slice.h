// This is mul/vil3d/vil3d_slice.h
#ifndef vil3d_slice_h_
#define vil3d_slice_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author Tim Cootes.

#include <vil2/vil2_image_view.h>
#include <vil3d/vil3d_image_view.h>
#include <vcl_cassert.h>


//: Return a 2D view of slice k of 3D image aligned as (j,i).
//  result(x,y,p)=im(y,x,k,p)
// \relates vil3d_image_view
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil3d_slice_ji(const vil3d_image_view<T> &im, unsigned k)
{
  assert(k<im.nk());
  return vil2_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+k*im.kstep(),
                            im.nj(),im.ni(),im.nplanes(),
                            im.jstep(),im.istep(),im.planestep());
}

//: Return a 2D view of slice k of 3D image aligned as (i,j)
//  result(x,y,p)=im(x,y,k,p)
// \relates vil3d_image_view
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil3d_slice_ij(const vil3d_image_view<T> &im, unsigned k)
{
  assert(k<im.nk());
  return vil2_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+k*im.kstep(),
                            im.ni(),im.nj(),im.nplanes(),
                            im.istep(),im.jstep(),im.planestep());
}

//: Return a 2D view of slice i of 3D image aligned as (j,k)
//  result(x,y,p)=im(i,x,y,p)
// \relates vil3d_image_view
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil3d_slice_jk(const vil3d_image_view<T> &im, unsigned i)
{
  assert(i<im.ni());
  return vil2_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+i*im.istep(),
                            im.nj(),im.nk(),im.nplanes(),
                            im.jstep(),im.kstep(),im.planestep());
}

//: Return a 2D view of slice i of 3D image aligned as (k,j)
//  result(x,y,p)=im(i,y,x,p)
// \relates vil3d_image_view
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil3d_slice_kj(const vil3d_image_view<T> &im, unsigned i)
{
  assert(i<im.ni());
  return vil2_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+i*im.istep(),
                            im.nk(),im.nj(),im.nplanes(),
                            im.kstep(),im.jstep(),im.planestep());
}

//: Return a 2D view of slice j of 3D image aligned as (k,i)
//  result(x,y,p)=im(y,i,x,p)
// \relates vil3d_image_view
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil3d_slice_ki(const vil3d_image_view<T> &im, unsigned j)
{
  assert(j<im.nj());
  return vil2_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+j*im.jstep(),
                            im.nk(),im.ni(),im.nplanes(),
                            im.kstep(),im.istep(),im.planestep());
}

//: Return a 2D view of slice j of 3D image aligned as (i,k)
//  result(x,y,p)=im(x,i,y,p)
// \relates vil3d_image_view
// \relates vil2_image_view
template<class T>
inline vil2_image_view<T> vil3d_slice_ik(const vil3d_image_view<T> &im, unsigned j)
{
  assert(j<im.nj());
  return vil2_image_view<T>(im.memory_chunk(),
                            im.origin_ptr()+j*im.jstep(),
                            im.ni(),im.nk(),im.nplanes(),
                            im.istep(),im.kstep(),im.planestep());
}

#endif // vil3d_slice_h_
