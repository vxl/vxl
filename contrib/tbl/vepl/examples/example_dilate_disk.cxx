//:
// \file
//  This example program shows a typical use of a morphological function on
//  a colour image.  The input image (argv[1]) must be 24 bit (colour), and in
//  that case is dilated (circular kernel, default 3+3 cross) to argv[2]
//  which is always a PPM file image.
//  Note that this requires the function max(rgbcell,rgbcell) which is provided
//  here (giving the pixel with the highest R value).
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil/vil_rgb.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vepl/vepl_dilate_disk.h>

typedef unsigned char ubyte;
typedef vil_rgb<ubyte> rgbcell;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_dilate_disk file_in file_out [radius]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);

  // The radius: (default is 3+3 cross)
  float radius = (argc < 4) ? 1 : atof(argv[3]);

  // The filter:
  vil_image out = vepl_dilate_disk(in,radius);

  // Write output:
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PNM to " << argv[2] << vcl_endl;

  return 0;
}
