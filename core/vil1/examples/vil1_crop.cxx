// This is core/vil1/examples/vil1_crop.cxx
// Example: image cropping

#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_save.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_crop.h>

int main(int argc, char ** argv)
{
  if (argc != 7) {
    std::cerr << "usage: vil1_crop in out x0 y0 width height\n";
    return -1;
  }
  char const* input_filename = argv[1];
  char const* output_filename = argv[2];
  int x0 = std::atoi(argv[3]);
  int y0 = std::atoi(argv[4]);
  int w = std::atoi(argv[5]);
  int h = std::atoi(argv[6]);

  vil1_image in = vil1_load(input_filename);

  // crop is a generic image whose get_section performs the crop.
  // so vil1_crop has size wxh rather than the size of the input image
  vil1_image crop = vil1_crop(in, x0, y0, w, h);

  vil1_save(crop, output_filename, in.file_format());
  return 0;
}

// save(crop, "t.pgm", "pnm") is implemented as:
//  (1) makes a file image, format "pnm" (i.e. a vil1_pnm_
//         dimensions,component type,size etc of "crop"
//  (2) vil1_copy(crop, fileimage)
