//:
// \file
//  This example program shows a typical use of the vipl_threshold IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_threshold<vnl_matrix<ubyte>,vnl_matrix<ubyte>,ubyte,ubyte>.
//  The conversion between vil_image and the in-memory vnl_matrix<ubyte>
//  is done explicitly.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <vnl/vnl_matrix.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vipl/vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h>
#include <vipl/vipl_threshold.h>

typedef unsigned char ubyte;
typedef vnl_matrix<ubyte> img_type;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example3_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_format(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<ubyte> out(in);

  // The image sizes:
  int xs = in.width();
  int ys = in.height();

  // The threshold value:
  ubyte threshold = (argc < 4) ? 10 : vcl_atoi(argv[3]);

  img_type src(xs,ys); // in-memory 2D vnl_matrix<ubyte>

  // set the input image:
  in.get_section(src.begin(),0,0,xs,ys);

  // perform thresholding:
  vipl_threshold<img_type,img_type,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op(threshold,0,255);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&src); // NOTE THAT dst == src
  op.filter();

  // Write output:
  out.put_section(src.begin(),0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
