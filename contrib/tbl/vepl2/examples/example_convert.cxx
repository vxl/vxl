//:
// \file
//  This example program shows a typical use of the vepl2_convert function on
//  any image.  The input image (argv[1]) is converted to a ubyte image
//  and written to argv[2] which is always a PGM file image.
//  When the input image is RGB, its intensity is extracted.
//  To this end an explicit converter from vil_rgb<ubyte> to ubyte is necessary.
//  When it is short, int or float, an implicit mapping is done to 0--255.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vil2/vil2_image_view.h>
#include <vil/vil_rgb.h>

#include <vepl2/vepl2_convert.h> // this one last!

// for I/O:
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vcl_iostream.h>

typedef vil_rgb<vxl_byte> rgbcell;

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_convert file_in file_out\n"; return 1; }

  // The input image:
  vil2_image_view_base_sptr in = vil2_load(argv[1]);

  // The output image:
  vil2_image_view<vxl_byte> out_grey(in);
  vil2_image_view<rgbcell> out_rgb(in);

  if (in->nplanes() == 1 && in->pixel_format() != VIL2_PIXEL_FORMAT_RGB_BYTE) { // monochrome
    if (vil2_pixel_format_sizeof_components(in->pixel_format()) == 1)
      vcl_cerr<<"Warning: no conversion necessary\n";
    out_grey = vepl2_convert(*in, (vxl_byte)0);
    vil2_save(out_grey, argv[2], "pnm");
    vcl_cout << "vepl2_convert()ed grey image to PGM image " << argv[2] << vcl_endl;
  } else if (in->nplanes() == 1) { // colour (RGB)
    vepl2_convert(*in,rgbcell());
    vil2_save(out_rgb, argv[2], "pnm");
    vcl_cout << "vepl2_convert()ed RGB image to PPM image " << argv[2] << vcl_endl;
  }
  else vcl_cerr << "Cannot handle image with "<< in->nplanes() <<" planes and format "<< in->pixel_format() << "\n";

  return 0;
}
