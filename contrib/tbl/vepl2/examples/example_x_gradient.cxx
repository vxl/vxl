//:
// \file
//  This example program shows a typical use of a gradient function on
//  a colour image.  The input image (argv[1]) must be 24 bit (colour), and in
//  that case its X gradient is written to argv[2] which is always a PPM file.
//  Note that this requires operator-() on the vil_rgb<ubyte> data type.
//  But this indeed produces a *colour* gradient!
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vepl2/vepl2_x_gradient.h>

// for I/O:
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vcl_iostream.h>

#include <vil/vil_rgb.h>
#include <vxl_config.h> // for vxl_byte
typedef vil_rgb<vxl_byte> rgbcell;

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_x_gradient file_in file_out\n";
    return 1;
  }

  // The input image:
  vil2_image_view<rgbcell> in = vil2_load(argv[1]);
  if (!in) { vcl_cerr<< "Please use a colour ubyte image as input\n"; return 2;}

  // The filter:
  vil2_image_view<rgbcell> out = vepl2_x_gradient(in);

  // Write output:
  vil2_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
