//:
// \file
//  This example program shows a typical use of the vepl2_histogram function on
//  an int image.  The input image (argv[1]) can be any scalar type (i.e.,
//  it should be mappable to int: [[un]signed] char, [unsigned]short, int),
//  and its histogram is calculated and written to stdout.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vepl2/vepl2_histogram.h>

// for I/O:
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_load.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv) {
  if (argc < 2)
  {
    vcl_cerr << "Syntax: example_histogram file_in\n";
    return 1;
  }

  // The input image:
  vil2_image_view<vxl_byte> in = vil2_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The filter:
  vcl_vector<unsigned int> out = vepl2_histogram(in);

  // Write output:
  for (unsigned int i=0; i<out.size(); ++i) if (out[i] != 0)
    vcl_cout << i << ": " << out[i] << vcl_endl;

  return 0;
}
