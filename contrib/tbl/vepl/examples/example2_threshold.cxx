// .NAME example_threshold
// .SECTION Description
//  This example program shows a typical use of the vepl_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 15 May 2001, from vipl/examples
//
#include <vil/vil_memory_image_of.h>
#include <vil/vil_pixel.h>

#include <vepl/vepl_threshold.h>

typedef unsigned char ubyte;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example2_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_format(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  int xs = in.width();
  int ys = in.height();

  // The threshold value:
  ubyte threshold = (argc < 4) ? 10 : atoi(argv[3]);

  // perform thresholding:
  vil_image out = vepl_threshold(in,threshold,0,255);

  // Write output:
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
