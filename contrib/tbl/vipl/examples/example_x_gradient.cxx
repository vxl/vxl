//:
// \file
//  This example program shows a typical use of a gradient IP class on
//  a colour image.  The input image (argv[1]) must be 24 bit (colour), and in
//  that case its X gradient is written to argv[2] which is always a PPM file.
//  Uses vipl_x_gradient<section<rgbcell,2>,section<rgbcell,2>,rgbcell,rgbcell>.
//  Note that this requires operator-() on the vil_rgb<ubyte> data type.
//  But this indeed produces a *colour* gradient!
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   29 may 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <section/section.h>
#include <vipl/vipl_with_section/accessors/vipl_accessors_section.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

#include <vipl/vipl_x_gradient.h>

typedef unsigned char ubyte;
typedef vil_rgb<ubyte> rgbcell;
typedef section<rgbcell,2> img_type;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_x_gradient file_in file_out\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_format(in) != VIL_RGB_BYTE) { vcl_cerr << "Please use a colour image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<rgbcell> out (in);
  
  // The image sizes:
  int xs = in.width();
  int ys = in.height();
  
  img_type src(xs,ys); // in-memory 2D images
  img_type dst(xs,ys);

  // set the input image:
  in.get_section(src.buffer,0,0,xs,ys);

  // The filter:
  vipl_x_gradient<img_type,img_type,rgbcell,rgbcell VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  out.put_section(dst.buffer,0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PPM to " << argv[2] << vcl_endl;

  return 0;
}
