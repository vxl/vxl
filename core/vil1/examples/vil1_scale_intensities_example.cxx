#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_clamp.h>
#include <vil/vil_image.h>
#include <vil/vil_image_as.h>
#include <vil/vil_pixel.h>
#include <vil/vil_scale_intensities.h>

int main (int argc, char** argv) {

  if (argc != 5) {
    vcl_cerr << __FILE__ " : scale shift in_image out_image" << vcl_endl;
    vcl_abort();
  }

  double scale = vcl_atof(argv[1]);
  double shift = vcl_atof(argv[2]);

  vil_image in = vil_load(argv[3]);

  if (vil_pixel_format(in) == VIL_BYTE) {
    vil_image real_img = vil_image_as_float(in);
    vil_image scaled_image = vil_scale_intensities(real_img, scale, shift);
    vil_save(vil_image_as_byte(vil_clamp(scaled_image, 0, 255)), argv[4], "pnm");
  }
  else if (vil_pixel_format(in) == VIL_RGB_BYTE) {
    vil_image real_img = vil_image_as_rgb_float(in);
    vil_image scaled_image = vil_scale_intensities(real_img, scale, shift);
    vil_save(vil_image_as_rgb_byte(vil_clamp(scaled_image, 0, 255)), argv[4], "pnm");
  }
  else
    ;

  return 0;
}
