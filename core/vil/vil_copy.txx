// This is mul/vil2/vil2_copy.txx
#ifndef vil2_smart_ptr_txx_
#define vil2_smart_ptr_txx_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

//:
// \file
// \author Ian Scott, ISBE, Manchester
// \date   4 Oct 2002


#include "vil2_copy.h"

//: Create a copy of the data viewed by this, and return a view of copy.
template<class T>
vil2_image_view<T> vil2_copy_deep(const vil2_image_view<T> &src)
{
  vil2_image_view<T> cpy;
  cpy.deep_copy(src);
  return cpy;
}


//: Copy src to dest, without changing dest's view parameters.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and types.
template<class T>
void vil2_copy_reformat(const vil2_image_view<T> &src, vil2_image_view<T> &dest)
{
  assert (src.nplanes() == dest.nplanes() &&
    src.nj() == dest.nj() &&
    src.ni() == dest.ni());
  for (unsigned p = 0; p < dest.nplanes(); ++p)
    for (unsigned j = 0; j < dest.nj(); ++j)
      for (unsigned i = 0; i < dest.ni(); ++i)
        dest(i,j,p) = src(i,j,p);
}


//: Copy src to window in dest.
// Size of window is defined by src.
//  O(window.size).
template<class T>
void vil2_copy_to_window(const vil2_image_view<T> &src, vil2_image_view<T> &dest,
                         unsigned i0, unsigned j0)
{
  // check window is within dest's bounds
  assert(i0+src.ni() <= dest.ni() && j0+src.nj() <= dest.nj());
  assert (src.nplanes() == dest.nplanes());

  for (unsigned p = 0; p < dest.nplanes(); ++p)
    for (unsigned j = 0; j < src.nj(); ++j)
      for (unsigned i = 0; i < src.ni(); ++i)
        dest(i+i0,j+j0,p) = src(i,j,p);
}


// For everything else
#define VIL2_COPY_INSTANTIATE(T) \
template void vil2_copy_to_window(const vil2_image_view<T > &src, vil2_image_view<T > &dest, \
  unsigned i0, unsigned j0); \
template void vil2_copy_reformat(const vil2_image_view<T > &src, vil2_image_view<T > &dest); \
template vil2_image_view<T > vil2_copy_deep(const vil2_image_view<T > &rhs);

#endif // vil2_smart_ptr_txx_
