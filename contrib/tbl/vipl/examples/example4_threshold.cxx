//:
// \file
//  This example program shows a typical use of the vipl_threshold IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_threshold<vil1_image,vil1_image,ubyte,ubyte>.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
// \endverbatim
//
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_pixel.h>
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_threshold.h>

#include <vxl_config.h> // for vxl_byte
typedef vil1_image img_type;

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example4_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  if (vil1_pixel_format(in) != VIL1_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil1_memory_image_of<vxl_byte> out(in);

  // The image sizes:
  int xs = in.width();
  int ys = in.height();

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 64 : vcl_atoi(argv[3]);

  vil1_memory_image_of<vxl_byte> src(in); // in-memory vil1_image
  vxl_byte* buf = new vxl_byte[in.get_size_bytes()];

  // set the input image:
  in.get_section(buf,0,0,xs,ys);
  src.put_section(buf,0,0,xs,ys);

  // perform thresholding:
  vipl_threshold<img_type,img_type,vxl_byte,vxl_byte> op(threshold,0,255);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&src); // NOTE THAT dst == src
  op.filter();

  // Write output:
  src.get_section(buf,0,0,xs,ys);
  out.put_section(buf,0,0,xs,ys);
  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  delete[] buf;
  return 0;
}
