//:
// \file
//  This example program shows a typical use of the vepl2_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_image_view.h>

#include <vepl2/vepl2_threshold.h>
#include <vcl_cstdlib.h> // for atoi()

typedef unsigned char ubyte;

// for I/O:
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example3_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil2_image_view_base_sptr in = vil2_load(argv[1]);
  if (in->pixel_format() != VIL2_PIXEL_FORMAT_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The threshold value:
  ubyte threshold = (argc < 4) ? 64 : vcl_atoi(argv[3]);

  // perform thresholding:
  vil2_image_view_base_sptr out = vepl2_threshold(*in,threshold,0,255);

  // Write output:
  vil2_save(*out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
