//:
// \file
//  This example program shows a typical use of the vepl_histogram function on
//  an int image.  The input image (argv[1]) can be any scalar type (i.e.,
//  it should be mappable to int: [[un]signed] char, [unsigned]short, int),
//  and its histogram is calculated and written to stdout.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vil/vil_pixel.h>
#include <vepl/vepl_histogram.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_image.h>
#include <vcl_iostream.h>

typedef unsigned char ubyte;

int
main(int argc, char** argv) {
  if (argc < 2) { vcl_cerr << "Syntax: example_histogram file_in\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);

  // The filter:
  vil_image out = vepl_histogram(in);

  // Write output:
  ubyte buf[256]; out.get_section(buf,0,0,256,1);
  for (int i=0; i<256; ++i) if (buf[i] != 0)
    vcl_cout << i << ": " << int(buf[i]) << vcl_endl;

  return 0;
}
