#ifndef vimt_image_pyramid_txx_
#define vimt_image_pyramid_txx_

//:
//  \file

#include "vimt_image_pyramid.h"

#include <vcl_cstdlib.h> // for abort()
#include <vcl_cassert.h>

//------------------------------------------------------------------------
//: Convert an image pyramid into a flat image containing each layer.
template <class T> void vimt_image_pyramid_flatten(T& out, const vimt_image_pyramid & in)
{
  const int n_levels = in.n_levels();
  assert (n_levels > 0);
  assert (out.is_a() == in(0).is_a());

  // cannot use dynamic_cast<> without rtti, which vxl doesn't enforce - PVr
#if 0
  if (dynamic_cast<vimt_image_2d *>(&out))
#else
  if (out.is_class("vimt_image_2d"))
#endif
  {
    unsigned width = 0;
    for (int i =0; i<n_levels; ++i)
      width += static_cast<const vimt_image_2d&>(in(i)).image_base().ni();

    const unsigned nplanes = static_cast<const vimt_image_2d&>(in(0)).image_base().nplanes();
    unsigned height = static_cast<const vimt_image_2d&>(in(0)).image_base().nj();
    out.image().set_size(width, height, nplanes);
    out.image().fill(0);
    int offset=0;
    for (int i =0; i<n_levels; ++i)
    {
      const T& im_i = static_cast<const T&>(in(i));
      const int ni = im_i.image().ni();
      const int nj = im_i.image().nj();

      for (int p=0;p<nplanes;++p)
        for (int y=0;y<nj;++y)
          for (int x=0;x<ni;++x)
          {
            out.image()(x+offset, y, p) = im_i.image()(x, y, p);
          }
      offset += ni;
    }
  }
  else
  {
    vcl_cerr << "ERROR: vimt_image_pyramid_flatten(const vimt_image_pyramid &)\n"
             << "Don't know how to deal with image type " << in(0).is_a()
             << vcl_endl;
    vcl_abort();
  }
}


#undef VIMT_IMAGE_PYRAMID_INSTANTIATE
#define VIMT_IMAGE_PYRAMID_INSTANTIATE(T) \
template void vimt_image_pyramid_flatten(T &, const vimt_image_pyramid &)

#endif //vimt_image_pyramid_txx_
