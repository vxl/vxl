//:
// \file
//  This example program shows a typical use of the vipl_threshold IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_threshold<vil_image,vil_image,ubyte,ubyte>.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <vil/vil_memory_image_of.h>
#include <vil/vil_pixel.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_threshold.h>

typedef unsigned char ubyte;
typedef vil_image img_type;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example4_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_format(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<ubyte> out(in);

  // The image sizes:
  int xs = in.width();
  int ys = in.height();

  // The threshold value:
  ubyte threshold = (argc < 4) ? 64 : vcl_atoi(argv[3]);

  vil_memory_image_of<ubyte> src(in); // in-memory vil_image
  ubyte* buf = new ubyte[in.get_size_bytes()];

  // set the input image:
  in.get_section(buf,0,0,xs,ys);
  src.put_section(buf,0,0,xs,ys);

  // perform thresholding:
  vipl_threshold<img_type,img_type,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op(threshold,0,255);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&src); // NOTE THAT dst == src
  op.filter();

  // Write output:
  src.get_section(buf,0,0,xs,ys);
  out.put_section(buf,0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  delete[] buf;
  return 0;
}
