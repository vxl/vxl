//:
// \file
//  This example program shows a typical use of the vipl_threshold IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_threshold<section<ubyte,2>,section<ubyte,2>,ubyte,ubyte>.
//  The conversion between vil_image_view<ubyte> and the in-memory section<ubyte,2>
//  is done explicitly.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_load by vil2_load
// \endverbatim
//
#include <section/section.h>
#include <vipl/vipl_with_section/accessors/vipl_accessors_section.h>

#include <vipl/vipl_threshold.h>

#include <vxl_config.h> // for vxl_byte
typedef section<vxl_byte,2> img_type;

// for I/O:
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()
#include <vcl_cstring.h> // for memcpy()

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example2_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_image_view<vxl_byte> out(in.ni(),in.nj(),in.nplanes());

  // The image sizes:
  int xs = in.ni();
  int ys = in.nj();

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 128 : vcl_atoi(argv[3]);

  img_type src(xs,ys); // in-memory 2D image

  // set the input image:
  vcl_memcpy(src.buffer, in.memory_chunk()->const_data(), in.size_bytes());

  // perform thresholding:
  vipl_threshold<img_type,img_type,vxl_byte,vxl_byte> op(threshold,0,255);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&src); // NOTE THAT dst == src
  op.filter();

  // Write output:
  vcl_memcpy(out.memory_chunk()->data(), src.buffer, out.size_bytes());
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
