#ifndef vil_resample_image_txx_
#define vil_resample_image_txx_

// This is vxl/vil/vil_resample_image.txx

#include "vil_resample_image.h"
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vil/vil_image.h>

template <class T, class A>
bool vil_resample_image(vil_image const &base, unsigned new_width, unsigned new_height,
                  T *buf, A* /*dummy*/,
                  int x0, int y0, int w, int h)
{
  // region is [x0, x1) x [y0, y1).
  int x1 = x0 + w;
  int y1 = y0 + h;
 
  // compute bounds of region needed from base image.
  unsigned base_x0 = x0*base.width()/new_width;
  unsigned base_x1 = x1*base.width()/new_width;
  unsigned base_w = base_x1 - base_x0;
  unsigned base_y0 = y0*base.height()/new_height;
  unsigned base_y1 = y1*base.height()/new_height;
  unsigned base_h = base_y1 - base_y0;
 
  // make buffer for, and get, region needed from base image.
  vcl_vector<T> base_buf(base_w * base_h);
  if (! base.get_section(/* xxx */&base_buf[0], base_x0, base_y0, base_w, base_h)) {
    vcl_cerr << __FILE__ ": get_section() failed on base image " << base << vcl_endl;
    return false;
  }
 
  // iterate over the destination buffer.
  for (int u=0; u<w; ++u) {
    int base_xlo = (x0+u  )*base.width()/new_width;
    int base_xhi = (x0+u+1)*base.width()/new_width;
 
    for (int v=0; v<h; ++v) {
      int base_ylo = (y0+v  )*base.height()/new_height;
      int base_yhi = (y0+v+1)*base.height()/new_height;
 
      A accum = 0;
      unsigned count = 0;
      for (int x=base_xlo; x<base_xhi; ++x) {
        for (int y=base_ylo; y<base_yhi; ++y) {
          accum += A(base_buf[(x-base_x0) + base_w*(y-base_y0)]);
          ++ count;
        }
      }
      if (count) accum /= count;
      buf[u + w*v] = T(accum);
    }
  }
  return true;
}

#undef VIL_RESAMPLE_IMAGE_INSTANTIATE
#define VIL_RESAMPLE_IMAGE_INSTANTIATE(T, A) \
template bool vil_resample_image(vil_image const &, unsigned, unsigned, T *, A *, int, int, int, int)

#endif
