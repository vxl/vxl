// .NAME example_threshold
// .SECTION Description
//  This example program shows a typical use of the vepl_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is thresholded (threshold value argv[3], default 127) to argv[2]
//  which is always a PGM file image.
//  The input and output images are directly passed to the filter
//  (be it that, except if NO_MEMORY_IMAGE is set, the input is buffered
//   into a vil_memory_image_of<ubyte>, because otherwise get_pixel()
//   would be very slow!)
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 15 May 2001, from vipl/examples
//
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>
#include <vepl/vepl_threshold.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()
typedef unsigned char ubyte;

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example1_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (!in) return 2;
#ifndef NO_MEMORY_IMAGE // otherwise get_pixel() would be very slow!!
  vil_memory_image_of<ubyte> mem(in);
  if (!mem) { vcl_cerr << "This is not a ubyte image\n"; return 3; }
  in = mem;
#endif

  // The threshold value:
  ubyte threshold = (argc < 4) ? 127 : vcl_atoi(argv[3]);

  // The filter:
  vil_image out = vepl_threshold(in,threshold,0);
  // without 4th argument, only set below threshold to 0

  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written thresholded image of type PNM to " << argv[2] << vcl_endl;
  return 0;
}
