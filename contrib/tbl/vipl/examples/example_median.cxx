// .NAME example_median
// .SECTION Description
//  This example program shows a typical use of the median filter on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case its median smoothed version (with circular kernel, default the
//  3x3 square) is written to argv[2] which is always a PGM file image.
//  Uses vipl_median<vnl_matrix<ubyte>,vnl_matrix<ubyte>,ubyte,ubyte>.
//  The conversion between vil_image and the in-memory vnl_matrix<ubyte>
//  is done explicitly.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 18 dec. 1998
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <vnl/vnl_matrix.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vipl/vipl_median.h>

typedef unsigned char ubyte;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { cerr << "Syntax: example_median file_in file_out [radius]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_type(in) != VIL_BYTE) { cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<ubyte> out(in);
  
  // The image sizes:
  int xs = in.width();
  int ys = in.height();
  
  // The radius: (default is 3x3 square)
  float radius = (argc < 4) ? 1.5 : atof(argv[3]);

  vnl_matrix<ubyte> src(xs,ys);
  vnl_matrix<ubyte> dst(xs,ys);

  // set the input image:
  in.get_section(src.begin(),0,0,xs,ys);

  // The filter:
  vipl_median<vnl_matrix<ubyte>,vnl_matrix<ubyte>,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op(radius);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  out.put_section(dst.begin(),0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  cout << "Written image of type PGM to " << argv[2] << endl;

  return 0;
}
