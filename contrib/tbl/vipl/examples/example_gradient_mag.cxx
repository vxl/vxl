//:
// \file
//  This example program shows a typical use of an IP filter, namely
//  the gradient magnitude operator on a greyscale image.  The input image
//  (argv[1]) must be a ubyte image, and in that case its gradient is
//  written to argv[2] which is always a PGM file image.
//  Uses vipl_gradient_mag<vbl_array_2d<ubyte>,vbl_array_2d<ubyte>,ubyte,ubyte>.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   29 may 1998.
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_load by vil2_load
// \endverbatim
//
#include <vbl/vbl_array_2d.h>

#include <vipl/vipl_with_vbl_array_2d/accessors/vipl_accessors_vbl_array_2d.h>
#include <vipl/vipl_gradient_mag.h>

#include <vxl_config.h> // for vxl_byte
typedef vbl_array_2d<vxl_byte> img_type;

// for I/O:
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h> // for memcpy()

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_gradient_mag file_in file_out\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_image_view<vxl_byte> out(in.ni(),in.nj(),in.nplanes());

  // The image sizes:
  int xs = in.ni();
  int ys = in.nj();

  img_type src(xs, ys);
  img_type dst(xs, ys);

  // set the input image:
  vcl_memcpy(src.begin(), in.memory_chunk()->const_data(), in.size_bytes());

  // The filter:
  vipl_gradient_mag<img_type,img_type,vxl_byte,vxl_byte> op;
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  vcl_memcpy(out.memory_chunk()->data(), dst.begin(), out.size_bytes());
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
