//:
// \file
//  This example program shows a typical use of a convolution filter, namely
//  the vepl_sobel (gradient) operator on a greyscale image.  The input image
//  (argv[1]) must be a ubyte image, and in that case its vepl_sobel image is
//  written to argv[2] which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/examples
//
#include <iostream>
#include <vepl/vepl_sobel.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    std::cerr << "Syntax: example_sobel file_in file_out\n";
    return 1;
  }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]);
  if (!in) { std::cerr << "Please use a ubyte image as input\n"; return 2; }

  // The filter:
  vil_image_resource_sptr out = vepl_sobel(in);

  // Write output:
  if (vil_save_image_resource(out, argv[2], "pnm"))
    std::cout << "Written sobel image to PNM image "<< argv[2]<< '\n';
  else
    std::cout << "Could not write sobel image as PNM to " << argv[2] << '\n';

  return 0;
}
