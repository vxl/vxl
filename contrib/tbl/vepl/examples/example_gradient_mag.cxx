//:
// \file
//  This example program shows a typical use of an IP filter, namely
//  the gradient magnitude operator on a greyscale image.  The input image
//  (argv[1]) must be a ubyte image, and in that case its gradient is
//  written to argv[2] which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vepl/vepl_gradient_mag.h>

typedef unsigned char ubyte;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_gradient_mag file_in file_out\n";
    return 1;
  }

  // The input image:
  vil_image in = vil_load(argv[1]);

  // The filter:
  vil_image out = vepl_gradient_mag(in);

  // Write output:
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
