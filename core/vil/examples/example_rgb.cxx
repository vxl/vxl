#include <vcl_iostream.h>
#include <vxl_config.h>
#include <vil/vil_rgb.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>

int main()
{
  // Load image into memory.
  vil_image_view<vil_rgb<vxl_byte> > img = vil_load("../tests/file_read_data/ff_rgb8bit_ascii.ppm");

  // Access pixel (1,2) and print its value as an int.
  vcl_cerr << "Pixel 1,2 = " << img(1,2) << vcl_endl;

  return 0;
}
