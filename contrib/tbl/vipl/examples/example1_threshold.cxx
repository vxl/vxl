//:
// \file
//  This example program shows a typical use of the vipl_threshold IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is thresholded (threshold value argv[3], default 127) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_threshold<vil1_image,vil1_image,ubyte,ubyte>.
//  The input and output images are directly passed to the filter
//  (be it that, except if NO_MEMORY_IMAGE is set, the input is buffered
//   into a vil1_memory_image_of<ubyte>, because otherwise get_pixel()
//   would be very slow!)
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_memory_image_of.h>
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_threshold.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()
typedef unsigned char ubyte;

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example1_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  if (!in) return 2;
  vil1_image* src;
#ifndef NO_MEMORY_IMAGE // otherwise get_pixel() would be very slow!!
  vil1_memory_image_of<ubyte> mem(in);
  if (!mem) { vcl_cerr << "This is not a ubyte image\n"; return 3; }
  src = &mem;
#else
  src = &in;
#endif

  // The output image:
  vil1_memory_image_of<ubyte> out(in);
  vil1_image* dst = &out;

  // The threshold value:
  ubyte threshold = (argc < 4) ? 127 : vcl_atoi(argv[3]);

  // The filter:
  vipl_threshold<vil1_image,vil1_image,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op(threshold,0);
  // without third argument, only set below threshold to 0
  op.put_in_data_ptr(src);
  op.put_out_data_ptr(dst);
  op.filter();

  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written thresholded image of type PGM to " << argv[2] << vcl_endl;
  return 0;
}
