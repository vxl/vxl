// This is tbl/vipl/examples/example_erode_disk.cxx

//:
// \file
//  This example program shows a typical use of a morphological IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is eroded (circular kernel, default 3x3 square) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_erode_disk<section<ubyte,2>,section<ubyte,2>,ubyte,ubyte>.
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

#include <vipl/vipl_erode_disk.h>

#include <vxl_config.h> // for vxl_byte
typedef section<vxl_byte,2> img_type;

// for I/O:
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()
#include <vcl_cstring.h> // for memcpy()

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_erode_disk file_in file_out [radius]\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_image_view<vxl_byte> out(in.ni(),in.nj(),in.nplanes());

  // The image sizes:
  int xs = in.ni();
  int ys = in.nj();

  // The radius: (default is 3x3 square)
  float radius = (argc < 4) ? 1.5f : (float)vcl_atof(argv[3]);

  img_type src(xs,ys); // in-memory 2D images
  img_type dst(xs,ys);

  // set the input image:
  vcl_memcpy(src.buffer, in.memory_chunk()->const_data(), in.size_bytes());

  // The filter:
  vipl_erode_disk<img_type,img_type,vxl_byte,vxl_byte> op(radius);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  vcl_memcpy(out.memory_chunk()->data(), dst.buffer, out.size_bytes());
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
