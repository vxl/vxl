//:
// \file
//  This example program shows a typical use of the vepl2_add_random_noise function
//  on a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case random noise is added to it (Gaussian, with sigma argv[3], default
//  1) to argv[2] which is always a PGM file image.
//  The input and output vil2_image_view_bases are directly passed to the filter.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vil2/vil2_image_view_base.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>

#include <vepl2/vepl2_add_random_noise.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_add_random_noise file_in file_out [width]\n"; return 1; }

  // The input image:
  vil2_image_view_base_sptr in = vil2_load(argv[1]);
  if (in->pixel_format() != VIL2_PIXEL_FORMAT_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The noise `width':
  double sigma = (argc < 4) ? 5 : vcl_atof(argv[3]);

  // The filter:
  vil2_image_view_base_sptr out = vepl2_add_random_noise(*in,sigma);

  vil2_save(*out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;
  return 0;
}
