//:
// \file
//  This example program shows a typical use of the vepl_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//  The conversion between vil1_image and the in-memory vnl_matrix<ubyte>
//  is done explicitly.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil1/vil1_pixel.h>
#include <vil1/vil1_memory_image_of.h>

#include <vepl/vepl_threshold.h>
#include <vxl_config.h> // for vxl_byte
#include <vcl_vector.h>

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atoi()

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example3_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  if (vil1_pixel_format(in) != VIL1_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  int xs = in.width();
  int ys = in.height();

  // In-memory version:
  vil1_memory_image_of<vxl_byte> out(in);
  vcl_vector<vxl_byte> buf(in.get_size_bytes());
  in.get_section(&buf[0],0,0,xs,ys);
  out.put_section(&buf[0],0,0,xs,ys);

  // The threshold value:
  vxl_byte threshold = (argc < 4) ? 64 : vcl_atoi(argv[3]);

  // perform thresholding:
  out = vepl_threshold(out,threshold,0,255); // NOTE THAT dst == src

  // Write output:
  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
