#ifndef vil_flatten_h_
#define vil_flatten_h_

// This is core/vil/vil_flatten.h
//:
// \file
// \author Ian Scott.

#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_plane.h>



//: Rearrange multiple planes into a multiple tiles of a single-plane image.
// A ni x nj x nplanes images will be arranged into an ni x (nj*nplanes) x 1 image.
// A view transformation will be used if possible.
template<class T>
inline vil_image_view<T> vil_flatten_planes(const vil_image_view<T> &im)
{
  if (im.nplanes() == 1) return im;
  if (im.jstep() * im.nj() == im.planestep())
  {
    return vil_image_view<T>(im.memory_chunk(),
                             im.top_left_ptr(),
                             im.ni(), im.nj() * im.nplanes(), 1,
                             im.istep(),im.jstep(),im.ni() * im.nj() * im.nplanes());
  }

  vil_image_view<T> ret(im.ni(), im.nj() * im.nplanes(), 1);
  for (unsigned p=0, n=im.nplanes(); p<n; ++p)
    vil_copy_reformat(vil_plane(im, p), vil_crop(ret, 0, im.ni(), im.nj()*p, im.nj()) );
}


#endif