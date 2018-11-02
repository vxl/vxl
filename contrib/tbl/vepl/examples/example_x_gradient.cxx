//:
// \file
//  This example program shows a typical use of a gradient function on
//  a colour image.  The input image (argv[1]) must be 24 bit (colour), and in
//  that case its X gradient is written to argv[2] which is always a PPM file.
//  Note that this requires operator-() on the vil_rgb<ubyte> data type.
//  But this indeed produces a \e colour gradient!
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/examples
//
#include <iostream>
#include <vepl/vepl_x_gradient.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_rgb.h>
#include <vxl_config.h> // for vxl_byte
typedef vil_rgb<vxl_byte> rgbcell;

int
main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Syntax: example_x_gradient file_in file_out\n";
    return 1;
  }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]);
  if (!in) { std::cerr<< "Please use a colour ubyte image as input\n"; return 2;}

  // The filter:
  vil_image_resource_sptr out = vepl_x_gradient(in);

  // Write output:
  if (vil_save_image_resource(out, argv[2], "pnm"))
    std::cout << "Written x-gradient image to PNM image "<< argv[2]<< '\n';
  else
    std::cout << "Could not write x-gradient image as PNM to " << argv[2] << '\n';

  return 0;
}
