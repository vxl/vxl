// .NAME example_gradient_mag
// .SECTION Description
//  This example program shows a typical use of an IP filter, namely
//  the gradient magnitude operator on a greyscale image.  The input image
//  (argv[1]) must be a ubyte image, and in that case its gradient is
//  written to argv[2] which is always a PGM file image.
//  Uses vipl_gradient_mag<vbl_array_2d<ubyte>,vbl_array_2d<ubyte>,ubyte,ubyte>.
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 29 may 1998.
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <vbl/vbl_array_2d.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vipl/vipl_gradient_mag.h>

typedef unsigned char ubyte;
typedef vbl_array_2d<ubyte> img_type;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { cerr << "Syntax: example_gradient_mag file_in file_out\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_type(in) != VIL_BYTE) { cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<ubyte> out(in);
  
  // The image sizes:
  int xs = in.width();
  int ys = in.height();
  
  img_type src(xs, ys);
  img_type dst(xs, ys);

  // set the input image:
  in.get_section(src.get_rows(),0,0,xs,ys);

  // The filter:
  vipl_gradient_mag<img_type,img_type,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  out.put_section(dst.get_rows(),0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  cout << "Written image of type PGM to " << argv[2] << endl;

  return 0;
}
