// This is tbl/vepl/examples/example_dilate_disk.cxx

//:
// \file
//  This example program shows a typical use of a morphological function on
//  a grey image.  The input image (argv[1]) must be 8 bit (grey), and in
//  that case is dilated (circular kernel, default 3+3 cross) to argv[2]
//  which is always a PGM file image.
//
//  Note that this cannot work with colour images unless a function
//  max(rgbcell,rgbcell) is provided.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vepl/vepl_dilate_disk.h>

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_dilate_disk file_in file_out [radius]\n";
    return 1;
  }

  // The input image:
  vil1_image in = vil1_load(argv[1]);

  // The radius: (default is 3+3 cross)
  float radius = (argc < 4) ? 1.0f : (float)vcl_atof(argv[3]);

  // The filter:
  vil1_image out = vepl_dilate_disk(in,radius);

  // Write output:
  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
