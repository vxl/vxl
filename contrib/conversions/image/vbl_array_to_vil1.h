#ifndef vbl_array_to_vil1_h_
#define vbl_array_to_vil1_h_

#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image.h>
#include <vbl/vbl_array_2d.h>

inline vil1_image vbl_array_to_vil1(vbl_array_2d<unsigned char> const& im)
{
  int width = im.columns();
  int height = im.rows();
  vil1_memory_image imo(width, height, 1, 8, VIL1_COMPONENT_FORMAT_UNSIGNED_INT);
  imo.put_section(im.begin(), 0, 0, width, height);
  return imo;
}

#endif // vbl_array_to_vil1_h_
