//:
// \file
//  This example program shows a typical use of the vepl_threshold function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is clipped (threshold value argv[3], default 10) to argv[2]
//  which is always a PGM file image.
//  The conversion between vil_image and the in-memory vnl_matrix<ubyte>
//  is done explicitly.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vepl/vepl_threshold.h>
#include <vcl_cstdlib.h> // for atoi()
#include <vcl_vector.h>

typedef unsigned char ubyte;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example3_threshold file_in file_out [threshold]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_format(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  int xs = in.width();
  int ys = in.height();

  // In-memory version:
  vil_memory_image_of<ubyte> out(in);
  vcl_vector<ubyte> buf(in.get_size_bytes());
  in.get_section(&buf[0],0,0,xs,ys);
  out.put_section(&buf[0],0,0,xs,ys);

  // The threshold value:
  ubyte threshold = (argc < 4) ? 64 : vcl_atoi(argv[3]);

  // perform thresholding:
  out = vepl_threshold(out,threshold,0,255); // NOTE THAT dst == src

  // Write output:
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
