#ifndef mil_image_pyramid_txx_
#define mil_image_pyramid_txx_

//:
//  \file

#include <mil/mil_image_pyramid.h>
#include <mil/mil_image.h>
#include <vcl_cstdlib.h> // for abort()

//------------------------------------------------------------------------
//: Convert an image pyramid into a flat image containing each layer.
template <class T> void mil_image_pyramid_flatten(T& out, const mil_image_pyramid & in)
{
  const int n_levels = in.n_levels();
  assert (n_levels > 0);
  assert (out.is_a() == in(0).is_a());

  // cannot use dynamic_cast<> without rtti, which vxl doesn't enforce - PVr
#if 0
  if (dynamic_cast<mil_image_2d *>(&out))
#else
  if (out.is_class("mil_image_2d"))
#endif
  {
    int width = 0;
    for (int i =0; i<n_levels; ++i)
      width += (in(i).nx());
    const int n_planes = in(0).n_planes();
    out.resize(width, in(0).ny(), n_planes);
    out.fill((typename T::pixel_type)0);
    int offset=0;
    for (int i =0; i<n_levels; ++i)
    {
      const int nx = in(i).nx();
      const int ny = in(i).ny();

      for (int p=0;p<n_planes;++p)
        for (int y=0;y<ny;++y)
          for (int x=0;x<nx;++x)
          {
            out(x+offset, y, p) = ((const T &)in(i))(x, y, p);
          }
      offset += nx;
    }
  }
  else 
  {
    vcl_cerr << "ERROR: mil_image_pyramid_flatten(const mil_image_pyramid &)\n"
             << "Don't know how to deal with image type " << in(0).is_a()
             << vcl_endl;
    vcl_abort();
  }
}


#undef MIL_IMAGE_PYRAMID_INSTANTIATE
#define MIL_IMAGE_PYRAMID_INSTANTIATE(T) \
template void mil_image_pyramid_flatten(T &, const mil_image_pyramid &)

#endif //mil_image_pyramid_txx_
