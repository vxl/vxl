// This is mul/vil3d/vil3d_crop.h
#ifndef vil3d_crop_h_
#define vil3d_crop_h_

#include <vil3d/vil3d_image_view.h>

//: Create a view that is a window onto an existing image.
// O(1).
template <class T>
vil3d_image_view<T> vil3d_crop(const vil3d_image_view<T>& im,
                               unsigned i0, unsigned ni, unsigned j0, unsigned nj,
										           unsigned k0, unsigned nk)
{
  assert(i0+ni<=im.ni());
  assert(j0+nj<=im.nj());
  assert(k0+nk<=im.nk());

  // Have to force the cast to avoid compiler warnings about const
  return vil3d_image_view<T>(im.memory_chunk(), &im(i0,j0,k0), ni, nj, nk,
    im.nplanes(), im.istep(), im.jstep(), im.kstep(), im.planestep());

}

#endif // vil3d_crop_h_
