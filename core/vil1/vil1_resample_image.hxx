// This is core/vil1/vil1_resample_image.hxx
#ifndef vil1_resample_image_hxx_
#define vil1_resample_image_hxx_

#include <vector>
#include <iostream>
#include "vil1_resample_image.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

#include <vil1/vil1_image.h>

template <class T, class A>
bool vil1_resample_image(vil1_image const &base, unsigned new_width, unsigned new_height,
                         T *buf, A* /*dummy*/,
                         int x0, int y0, int w, int h)
{
  assert(new_width > 0 && new_height > 0);

  // region is [x0, x1) x [y0, y1).
  int x1 = x0 + w;
  int y1 = y0 + h;

  // scale factor between the two images
  const double fx = base.width()/(double)new_width;
  const double fy = base.height()/(double)new_height;

  // compute bounds of region needed from base image.
  unsigned base_x0 = int(x0 * fx);
  unsigned base_x1 = int(x1 * fx);
  unsigned base_w = base_x1 - base_x0 + 1;
  unsigned base_y0 = int(y0 * fy);
  unsigned base_y1 = int(y1 * fy);
  unsigned base_h = base_y1 - base_y0 + 1;

  // make buffer for, and get, region needed from base image.
  std::vector<T> base_buf(base_w * base_h);
  if (! base.get_section(/* xxx */&base_buf[0], base_x0, base_y0, base_w, base_h)) {
    std::cerr << __FILE__ ": get_section() failed on base image " << base <<'\n';
    return false;
  }

  // "e" is as close as possible to 1.
  constexpr double e = 0.999999999999;

  // iterate over the destination buffer.
  for (int u=0; u<w; ++u) {
    int base_xlo = int((x0+u  ) * fx) - base_x0;
    int base_xhi = int((x0+u+e) * fx) - base_x0;

    for (int v=0; v<h; ++v) {
      int base_ylo = int((y0+v  ) * fy) - base_y0;
      int base_yhi = int((y0+v+e) * fy) - base_y0;

      A accum = 0;
      unsigned count = 0;
      for (int x=base_xlo; x<=base_xhi; ++x) {
        for (int y=base_ylo; y<=base_yhi; ++y) {
          accum += A(base_buf[x + base_w*y]);
          ++ count;
        }
      }
      assert(count!=0); // count cannot be zero: above for loop is never empty.
      accum /= count;
      buf[u + w*v] = T(accum);
    }
  }
  return true;
}

#undef VIL1_RESAMPLE_IMAGE_INSTANTIATE
#define VIL1_RESAMPLE_IMAGE_INSTANTIATE(T, A) \
template bool vil1_resample_image(vil1_image const &, unsigned, unsigned, T *, A *, int, int, int, int)

#endif
