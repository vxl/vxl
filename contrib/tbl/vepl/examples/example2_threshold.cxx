// This is tbl/vepl/examples/example2_threshold.cxx

//:
// \file
//  This example program shows a typical use of the vepl_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil1/vil1_pixel.h>

#include <vepl/vepl_threshold.h>

#include <vxl_config.h> // for vxl_byte

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example2_threshold file_in file_out [threshold]\n";
    return 1;
  }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  if (vil1_pixel_format(in) != VIL1_BYTE)
  {
    vcl_cerr << "Please use a ubyte image as input\n";
    return 2;
  }

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 128 : vcl_atoi(argv[3]);

  // perform thresholding:
  vil1_image out = vepl_threshold(in,threshold,0,255);

  // Write output:
  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
