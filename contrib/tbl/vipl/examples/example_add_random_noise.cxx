//:
// \file
//  This example program shows a typical use of the vipl_add_random_noise IP class
//  on a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case random noise is added to it (Gaussian, with sigma argv[3], default
//  1) to argv[2] which is always a PGM file image.
//  Uses vipl_add_random_noise<vil_image_view<ubyte>,vil_image_view<ubyte>,ubyte,ubyte>.
//  The input and output vil_image_views are directly passed to the filter.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   28 may 1998
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim
//
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/vipl_add_random_noise.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()
#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_add_random_noise file_in file_out [width]\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_image_view<vxl_byte> out(in.ni(),in.nj(),in.nplanes());

  // The noise `width':
  double sigma = (argc < 4) ? 5.0 : vcl_atof(argv[3]);
  vxl_byte s = (vxl_byte)(sigma+0.5); // round to integer

  // The filter:
  vipl_add_random_noise<vil_image_view<vxl_byte>,vil_image_view<vxl_byte>,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,s);
  op.put_in_data_ptr(&in);
  op.put_out_data_ptr(&out);
  op.filter();

  vil_save(out, argv[2]);
  vcl_cout << "Noisy image written to " << argv[2] << vcl_endl;
  return 0;
}
