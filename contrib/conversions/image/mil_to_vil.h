#ifndef mil_to_vil_h_
#define mil_to_vil_h_

#include <vil/vil_image.h>
#include <mil/mil_convert_vil.h>

template <class T>
inline vil_image mil_to_vil(mil_image_2d_of<T> const& im)
{
  assert(im.n_dims() == 2);
  if (im.n_planes() == 1) {
    vil_memory_image_of<T> imo;
    mil_convert_vil_gm2gv(imo, im);
    return imo;
  }
  else if (im.n_planes() == 3) {
    vil_memory_image_of<vil_rgb_byte> imo;
    mil_convert_vil_cm2cv(imo, im);
    return imo;
  }
  else assert(!"Can only convert 1-plane or 3-plane mil images");
  return 0;
}

#endif // mil_to_vil_h_
