#ifndef vbl_array_to_vil_h_
#define vbl_array_to_vil_h_

#include <vil/vil_image.h>
#include <vil/vil_memory_image.h>
#include <vbl/vbl_array_2d.h>

inline vil_image vbl_array_to_vil(vbl_array_2d<unsigned char> const& im)
{
  int width = im.columns();
  int height = im.rows();
  vil_memory_image imo(width, height, 1, 8, VIL_COMPONENT_FORMAT_UNSIGNED_INT);
  imo.put_section(im.begin(), 0, 0, width, height);
  return imo;
}

#endif // vbl_array_to_vil_h_
