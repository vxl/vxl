//:
// \file
//  This example program shows a typical use of the vipl_add_random_noise IP class
//  on a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case random noise is added to it (Gaussian, with sigma argv[3], default
//  1) to argv[2] which is always a PGM file image.
//  Uses vipl_add_random_noise<vil1_image,vil1_image,ubyte,ubyte>.
//  The input and output vil1_images are directly passed to the filter
//  (be it that, except if NO_MEMORY_IMAGE is set, the input is buffered
//   into a vil1_memory_image_of<ubyte>, because otherwise get_pixel()
//   would be very slow!)
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   28 may 1998
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <vil1/vil1_image.h>
#include <vil1/vil1_pixel.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_memory_image_of.h>

#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_add_random_noise.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

typedef unsigned char ubyte;

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_add_random_noise file_in file_out [width]\n"; return 1; }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  if (vil1_pixel_format(in) != VIL1_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  vil1_image* src;
#ifndef NO_MEMORY_IMAGE // otherwise get_pixel() would be very slow!!
  vil1_memory_image_of<ubyte> mem (in);
  src = &mem;
#else
  src = &in;
#endif

  // The output image:
  vil1_memory_image_of<ubyte> out(*src);
  vil1_image* dst = &out;

  // The noise `width':
  double sigma = (argc < 4) ? 5.0 : vcl_atof(argv[3]);
  ubyte s = (ubyte)(sigma+0.5); // round to integer

  // The filter:
  vipl_add_random_noise<vil1_image,vil1_image,ubyte,ubyte> op(GAUSSIAN_NOISE,s);
  op.put_in_data_ptr(src);
  op.put_out_data_ptr(dst);
  op.filter();

  vil1_save(out, argv[2]);
  vcl_cout << "Noisy image written to " << argv[2] << vcl_endl;
  return 0;
}
