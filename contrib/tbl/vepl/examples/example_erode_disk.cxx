//:
// \file
//  This example program shows a typical use of a morphological function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is eroded (circular kernel, default 3x3 square) to argv[2]
//  which is always a PGM file image.
//  The conversion between vil_image and the in-memory section<ubyte,2>
//  is done explicitly.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vepl/vepl_erode_disk.h>

typedef unsigned char ubyte;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_erode_disk file_in file_out [radius]\n";
    return 1;
  }

  // The input image:
  vil_image in = vil_load(argv[1]);

  // The radius: (default is 3x3 square)
  float radius = (argc < 4) ? 1.5 : atof(argv[3]);

  // The filter:
  vil_image out = vepl_erode_disk(in,radius);

  // Write output:
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
