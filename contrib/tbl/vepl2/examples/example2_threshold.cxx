// This is tbl/vepl2/examples/example2_threshold.cxx

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
#include <vepl2/vepl2_threshold.h>
#include <vxl_config.h> // for vxl_byte

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
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
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 128 : vcl_atoi(argv[3]);

  // perform thresholding:
  vil_image_resource_sptr out = vepl2_threshold(in,threshold,0,255);

  // Write output:
  if (vil_save_image_resource(out, argv[2], "pnm"))
    vcl_cout << "Written thresholded image of type PNM to " << argv[2] << '\n';
  else
    vcl_cout << "Could not write thresholded image as PNM to "<< argv[2]<<'\n';

  return 0;
}
