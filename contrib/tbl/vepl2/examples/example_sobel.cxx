//:
// \file
//  This example program shows a typical use of a convolution filter, namely
//  the vepl2_sobel (gradient) operator on a greyscale image.  The input image
//  (argv[1]) must be a ubyte image, and in that case its vepl2_sobel image is
//  written to argv[2] which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vepl2/vepl2_sobel.h>

typedef unsigned char ubyte;

// for I/O:
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_sobel file_in file_out\n";
    return 1;
  }

  // The input image:
  vil2_image_view_base_sptr in = vil2_load(argv[1]);

  // The filter:
  vil2_image_view_base_sptr out = vepl2_sobel(*in);

  // Write output:
  vil2_save(*out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
