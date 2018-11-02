//:
// \file
//  This example program shows a typical use of the vepl_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/examples
//
#include <iostream>
#include <cstdlib>
#include <vepl/vepl_threshold.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>

int
main(int argc, char** argv)
{
  if (argc < 3) { std::cerr << "Syntax: example3_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]);
  if (!in) { std::cerr << "Please use a ubyte image as input\n"; return 2; }

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 64 : vxl_byte(std::atoi(argv[3]));

  // perform thresholding:
  vil_image_resource_sptr out = vepl_threshold(in,threshold,0,255);

  // Write output:
  if (vil_save_image_resource(out, argv[2], "pnm"))
    std::cout << "Written thresholded image of type PNM to " << argv[2] << '\n';
  else
    std::cout << "Could not write thresholded image as PNM to "<< argv[2]<<'\n';

  return 0;
}
