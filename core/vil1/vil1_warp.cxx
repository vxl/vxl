
#include <vcl_cassert.h>

#include <vil/vil_warp.h>
#include <vil/vil_warp.txx>

#include <vil/vil_rgb.h>
#include <vil/vil_pixel.h>

VIL_WARP_INSTANTIATE(unsigned char, vil_warp_mapping);
VIL_WARP_INSTANTIATE(vil_rgb<unsigned char>, vil_warp_mapping);

vil_image vil_warp(vil_image const& in, vil_warp_mapping const& mapper,
		   vil_warp_interpolation_type i)
{
  if (vil_pixel_format(in) == VIL_BYTE) {
    vil_memory_image_of<unsigned char> inimg(in);
    vil_memory_image_of<unsigned char> outimg(in.width(), in.height());
    vil_warp_output_driven(inimg, outimg, mapper, i);
    return outimg;

  } else if (vil_pixel_format(in) == VIL_RGB_BYTE) {
    vil_memory_image_of<vil_rgb<unsigned char> > inimg(in);
    vil_memory_image_of<vil_rgb<unsigned char> > outimg(in.width(), in.height());
    vil_warp_output_driven(inimg, outimg, mapper, i);
    return outimg;
    
  } else {
    assert(0);
    return vil_image();
  }
}
