//:
// \file
//  This example program shows a typical use of the vepl_convert function on
//  any image.  The input image (argv[1]) is converted to a ubyte image
//  and written to argv[2] which is always a PGM file image.
//  When the input image is RGB, its intensity is extracted.
//  To this end an explicit converter from vil1_rgb<ubyte> to ubyte is necessary.
//  When it is short, int or float, an implicit mapping is done to 0--255.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

#include <vepl/vepl_convert.h> // this one last!

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>
#include <vil1/vil1_rgb.h>
#include <vil1/vil1_memory_image_of.txx>

#include <vxl_config.h> // for vxl_byte
class rgbcell: public vil1_rgb<vxl_byte>
{
public:
  rgbcell() {};
  rgbcell(double x): vil1_rgb<vxl_byte>(vxl_byte(x)) {}
};

VIL1_MEMORY_IMAGE_OF_INSTANTIATE(vil1_rgb<double>);

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_convert file_in file_out\n"; return 1; }

  // The input image:
  vil1_image in = vil1_load(argv[1]);

  // The output image:
  vil1_memory_image_of<vxl_byte> out_grey(in);
  vil1_memory_image_of<rgbcell> out_rgb(in);

  if (in.planes() == 1 && in.components() == 1) { // monochrome
    vxl_byte dummy = 0;
    if (in.bits_per_component() == 8)
      vcl_cerr<<"Warning: no conversion necessary\n";
    out_grey = vepl_convert(in, dummy);
    vil1_save(out_grey, argv[2], "pnm");
    vcl_cout << "vepl_convert()ed grey image to PGM image " << argv[2] << vcl_endl;
  } else if (in.planes() == 1 && in.components() == 3) { // colour (RGB)
    out_rgb = vepl_convert(in,rgbcell());
    vil1_save(out_rgb, argv[2], "pnm");
    vcl_cout << "vepl_convert()ed RGB image to PPM image " << argv[2] << vcl_endl;
  }
  else vcl_cerr << "Cannot handle image with "<< in.planes() <<" planes and "<< in.components() <<" components\n";

  return 0;
}
