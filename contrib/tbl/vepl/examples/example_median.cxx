// This is tbl/vepl/examples/example_median.cxx

//:
// \file
//  This example program shows a typical use of the median filter on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case its median smoothed version (with circular kernel, default the
//  3x3 square) is written to argv[2] which is always a PGM file image.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vepl/vepl_median.h>

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_median file_in file_out [radius]\n";
    return 1;
  }

  // The input image:
  vil1_image in = vil1_load(argv[1]);

  // The radius: (default is 3x3 square)
  float radius = (argc < 4) ? 1.5f : (float)vcl_atof(argv[3]);

  // The filter:
  vil1_image out = vepl_median(in,radius);

  // Write output:
  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
