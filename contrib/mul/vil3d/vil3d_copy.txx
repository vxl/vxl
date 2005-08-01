// This is mul/vil3d/vil3d_copy.txx
#ifndef vil3d_copy_txx_
#define vil3d_copy_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott, ISBE, Manchester
// \date   2 Feb 2003

#include "vil3d_copy.h"
#include <vcl_cassert.h>
#include <vil3d/vil3d_image_view.h>

//: Create a copy of the data viewed by this, and return a view of copy.
template<class T>
void vil3d_copy_deep(const vil3d_image_view<T> &src, vil3d_image_view<T> &dest)
{
  dest.deep_copy(src);
}

//: Create a copy of the data viewed by this, and return a view of copy.
template<class T>
vil3d_image_view<T> vil3d_copy_deep(const vil3d_image_view<T> &src)
{
  vil3d_image_view<T> cpy;
  cpy.deep_copy(src);
  return cpy;
}


//: Copy src to dest, without changing dest's view parameters.
// This is useful if you want to copy an image into a window on another image.
// src and dest must have identical sizes, and types.
template<class T>
void vil3d_copy_reformat(const vil3d_image_view<T> &src, vil3d_image_view<T> &dest)
{
  assert (src.nplanes() == dest.nplanes() &&
          src.nk() == dest.nk() && src.nj() == dest.nj() &&
          src.ni() == dest.ni());
  for (unsigned p = 0; p < dest.nplanes(); ++p)
    for (unsigned k = 0; k < dest.nk(); ++k)
      for (unsigned j = 0; j < dest.nj(); ++j)
        for (unsigned i = 0; i < dest.ni(); ++i)
          dest(i,j,k,p) = src(i,j,k,p);
}


//: Copy src to window in dest.
// Size of window is defined by src.
//  O(window.size).
template<class T>
void vil3d_copy_to_window(const vil3d_image_view<T> &src, vil3d_image_view<T> &dest,
                         unsigned i0, unsigned j0, unsigned k0)
{
  // check window is within dest's bounds
  assert(i0+src.ni() <= dest.ni() && j0+src.nj() <= dest.nj() && k0+src.nk() <= dest.nk());
  assert (src.nplanes() == dest.nplanes());

  for (unsigned p = 0; p < dest.nplanes(); ++p)
    for (unsigned k = 0; k < src.nk(); ++k)
      for (unsigned j = 0; j < src.nj(); ++j)
        for (unsigned i = 0; i < src.ni(); ++i)
          dest(i+i0,j+j0,k+k0,p) = src(i,j,k,p);
}


// For everything else
#define VIL3D_COPY_INSTANTIATE(T) \
template void vil3d_copy_to_window(const vil3d_image_view<T > &src, vil3d_image_view<T > &dest, \
                                  unsigned i0, unsigned j0, unsigned k0); \
template void vil3d_copy_reformat(const vil3d_image_view<T > &src, vil3d_image_view<T > &dest); \
template vil3d_image_view<T > vil3d_copy_deep(const vil3d_image_view<T > &rhs)

#endif // vil3d_copy_txx_
