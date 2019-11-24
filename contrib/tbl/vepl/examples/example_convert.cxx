//:
// \file
//  This example program shows a typical use of the vepl_convert function on
//  any image.  The input image (argv[1]) is converted to a ubyte image
//  and written to argv[2] which is always a PGM file image.
//  When the input image is RGB, its intensity is extracted.
//  To this end an explicit converter from vil_rgb<ubyte> to ubyte is necessary.
//  When it is short, int or float, an implicit mapping is done to 0--255.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/examples
//
#include <iostream>
#include <vepl/vepl_convert.h>
#include "vil/vil_rgb.h"

// for I/O:
#include "vil/vil_load.h"
#include "vil/vil_save.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vxl_config.h" // for vxl_byte
#include "vil/vil_image_view.hxx"

class rgbcell: public vil_rgb<vxl_byte>
{
 public:
  rgbcell() = default;
  rgbcell(double x): vil_rgb<vxl_byte>(vxl_byte(x)) {}
};

VIL_IMAGE_VIEW_INSTANTIATE(rgbcell);

int
main(int argc, char** argv)
{
  if (argc < 3) { std::cerr << "Syntax: example_convert file_in file_out\n"; return 1; }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]);

  if (in->nplanes() == 1 && in->pixel_format() == vil_pixel_format_component_format(in->pixel_format()))
  { // monochrome
    if (vil_pixel_format_sizeof_components(in->pixel_format()) == 1)
      std::cerr << "Warning: no conversion necessary\n";
    vil_image_resource_sptr out = vepl_convert(in, (vxl_byte)0);
    if (vil_save_image_resource(out, argv[2], "pnm"))
      std::cout << "vepl_convert()ed grey image to PGM image "<< argv[2]<< '\n';
    else
      std::cout << "Could not convert grey image as PGM to " << argv[2] << '\n';
  }
  else if (in->nplanes() == 1 || in->nplanes() == 3) // colour (RGB)
  {
    vil_image_resource_sptr out = vepl_convert(in,rgbcell());
    if (vil_save_image_resource(out, argv[2], "pnm"))
      std::cout << "vepl_convert()ed RGB image to PPM image "<< argv[2] << '\n';
    else
      std::cout << "Could not convert RGB image as PPM to " << argv[2] << '\n';
  }
  else std::cerr << "Cannot handle image with "<< in->nplanes() <<" planes and format "<< in->pixel_format() << '\n';

  return 0;
}
