//:
// \file
//  This example program shows a typical use of the vipl_threshold IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is thresholded (threshold value argv[3], default 127) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_threshold<vil_image_view<ubyte>,vil_image_view<ubyte>,ubyte,ubyte>.
//  The input and output images are directly passed to the filter.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim
//
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/vipl_threshold.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()
#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example1_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) return 2;
  vil_image_view<vxl_byte>* src = &in;

  // The output image:
  vil_image_view<vxl_byte> out(in.ni(),in.nj(),in.nplanes());
  vil_image_view<vxl_byte>* dst = &out;

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 127 : vcl_atoi(argv[3]);

  // The filter:
  vipl_threshold<vil_image_view<vxl_byte>,vil_image_view<vxl_byte>,vxl_byte,vxl_byte> op(threshold,0);
  // without third argument, only set below threshold to 0
  op.put_in_data_ptr(src);
  op.put_out_data_ptr(dst);
  op.filter();

  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written thresholded image of type PGM to " << argv[2] << vcl_endl;
  return 0;
}
