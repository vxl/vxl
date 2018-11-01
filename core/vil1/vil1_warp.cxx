// This is core/vil1/vil1_warp.cxx

#include "vil1_warp.h"
#include "vil1_warp.hxx"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_rgb.h>
#include <vil1/vil1_pixel.h>

VIL1_WARP_INSTANTIATE(unsigned char, vil1_warp_mapping);
VIL1_WARP_INSTANTIATE(vil1_rgb<unsigned char>, vil1_warp_mapping);
VIL1_WARP_INSTANTIATE(double, vil1_warp_mapping);

vil1_image vil1_warp(vil1_image const& in, vil1_warp_mapping const& mapper,
                     vil1_warp_interpolation_type i,
                     int out_width, int out_height)
{
  int ow = out_width  < 0 ? in.width()  : out_width;
  int oh = out_height < 0 ? in.height() : out_height;

  if (vil1_pixel_format(in) == VIL1_BYTE) {
    vil1_memory_image_of<unsigned char> inimg(in);
    vil1_memory_image_of<unsigned char> outimg(ow, oh);
    vil1_warp_output_driven(inimg, outimg, mapper, i);
    return outimg;

  } else if (vil1_pixel_format(in) == VIL1_RGB_BYTE) {
    vil1_memory_image_of<vil1_rgb<unsigned char> > inimg(in);
    vil1_memory_image_of<vil1_rgb<unsigned char> > outimg(ow, oh);
    vil1_warp_output_driven(inimg, outimg, mapper, i);
    return outimg;

  } else if (vil1_pixel_format(in) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> inimg(in);
    vil1_memory_image_of<double> outimg(ow, oh);
    vil1_warp_output_driven(inimg, outimg, mapper, i);
    return outimg;

  } else {
    assert(0);
    return vil1_image();
  }
}
