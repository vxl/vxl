// This is vxl/vil/examples/vil_crop.cxx
// Example: image cropping

#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()

#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>

int main(int argc, char ** argv)
{
  if (argc != 7) {
    vcl_cerr << "usage: vil_crop in out x0 y0 width height\n";
    return -1;
  }
  char const* input_filename = argv[1];
  char const* output_filename = argv[2];
  int x0 = vcl_atoi(argv[3]);
  int y0 = vcl_atoi(argv[4]);
  int w = vcl_atoi(argv[5]);
  int h = vcl_atoi(argv[6]);
 
  vil_image in = vil_load(input_filename);

  // crop is a generic image whose get_section performs the crop.
  // so vil_crop has size wxh rather than the size of the input image
  vil_image crop = vil_crop(in, x0, y0, w, h);

  vil_save(crop, output_filename, in.file_format());
  return 0;
}

// save(crop, "t.pgm", "pnm") is implemented as:
//  (1) makes a file image, format "pnm" (i.e. a vil_pnm_
//         dimensions,component type,size etc of "crop"
//  (2) vil_copy(crop, fileimage)
