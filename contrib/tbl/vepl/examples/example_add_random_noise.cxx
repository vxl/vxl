//:
// \file
//  This example program shows a typical use of the vepl_add_random_noise function
//  on a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case random noise is added to it (Gaussian, with sigma argv[3], default
//  1) to argv[2] which is always a PGM file image.
//  The input and output vil1_images are directly passed to the filter
//  (be it that, except if NO_MEMORY_IMAGE is set, the input is buffered
//   into a vil1_memory_image_of<ubyte>, because otherwise get_pixel()
//   would be very slow!)
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil1/vil1_image.h>
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vil1/vil1_memory_image_of.h>

#include <vepl/vepl_add_random_noise.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()
#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_add_random_noise file_in file_out [width]\n"; return 1; }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  vil1_image& src = in;
#ifndef NO_MEMORY_IMAGE // otherwise get_pixel() would be very slow!!
  vil1_memory_image_of<vxl_byte> mem(in);
  src = mem;
#endif

  // The noise `width':
  double sigma = (argc < 4) ? 5 : vcl_atof(argv[3]);

  // The filter:
  vil1_image out = vepl_add_random_noise(src,sigma);

  vil1_save(out, argv[2]);
  vcl_cout << "Noisy image written to " << argv[2] << vcl_endl;
  return 0;
}
