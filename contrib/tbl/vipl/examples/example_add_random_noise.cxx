// .NAME example_add_random_noise
// .SECTION Description
//  This example program shows a typical use of the vipl_add_random_noise IP class
//  on a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case random noise is added to it (Gaussian, with sigma argv[3], default
//  1) to argv[2] which is always a PGM file image.
//  Uses vipl_add_random_noise<vil_image,vil_image,ubyte,ubyte>.
//  The input and output vil_images are directly passed to the filter
//  (be it that, except if NO_MEMORY_IMAGE is set, the input is buffered
//   into a vil_memory_image_of<ubyte>, because otherwise get_pixel()
//   would be very slow!)
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 28 may 1998
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <vil/vil_image.h>
#include <vil/vil_pixel.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_memory_image_of.h>

#include <vipl/vipl_add_random_noise.h>
#include <vcl_iostream.h>

typedef unsigned char ubyte;

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_add_random_noise file_in file_out [width]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_type(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  vil_image* src = &in;
#ifndef NO_MEMORY_IMAGE // otherwise get_pixel() would be very slow!!
  vil_memory_image_of<ubyte> mem (in);
  src = &mem;
#endif

  // The output image:
  vil_memory_image_of<ubyte> out(*src);
  vil_image* dst = &out;
  
  // The noise `width':
  double sigma = (argc < 4) ? 1 : atof(argv[3]);

  // The filter:
  vipl_add_random_noise<vil_image,vil_image,ubyte,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op(GAUSSIAN_NOISE,sigma);
  op.put_in_data_ptr(src);
  op.put_out_data_ptr(dst);
  op.filter();

  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;
  return 0;
}
