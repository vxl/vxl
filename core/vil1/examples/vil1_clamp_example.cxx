#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_clamp.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_image_as.h>
#include <vil1/vil1_pixel.h>

int main (int argc, char** argv)
{
  if (argc != 5) {
    vcl_cerr << __FILE__ " : low_clamp high_clamp in_image out_image\n";
    vcl_abort();
  }

  double low  = vcl_atof(argv[1]);
  double high = vcl_atof(argv[2]);

  vil1_image in = vil1_load(argv[3]);
  if (vil1_pixel_format(in) == VIL1_BYTE)
    vil1_save(vil1_image_as_byte(vil1_clamp(in, low, high)), argv[4], "pnm");
  else if (vil1_pixel_format(in) == VIL1_RGB_BYTE)
    vil1_save(vil1_image_as_rgb_byte(vil1_clamp(in, low, high)), argv[4], "pnm");

  return 0;
}
