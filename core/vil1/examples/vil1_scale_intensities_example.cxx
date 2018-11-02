#include <cstdlib>
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_clamp.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_image_as.h>
#include <vil1/vil1_pixel.h>
#include <vil1/vil1_scale_intensities.h>

int main (int argc, char** argv)
{
  if (argc != 5) {
    std::cerr << __FILE__ " : scale shift in_image out_image\n";
    std::abort();
  }

  double scale = std::atof(argv[1]);
  double shift = std::atof(argv[2]);

  vil1_image in = vil1_load(argv[3]);

  if (vil1_pixel_format(in) == VIL1_BYTE) {
    vil1_image real_img = vil1_image_as_float(in);
    vil1_image scaled_image = vil1_scale_intensities(real_img, scale, shift);
    vil1_save(vil1_image_as_byte(vil1_clamp(scaled_image, 0, 255)), argv[4], "pnm");
  }
  else if (vil1_pixel_format(in) == VIL1_RGB_BYTE) {
    vil1_image real_img = vil1_image_as_rgb_float(in);
    vil1_image scaled_image = vil1_scale_intensities(real_img, scale, shift);
    vil1_save(vil1_image_as_rgb_byte(vil1_clamp(scaled_image, 0, 255)), argv[4], "pnm");
  }

  return 0;
}
