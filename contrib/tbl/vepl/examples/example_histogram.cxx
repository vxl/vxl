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
#include <vepl/vepl_histogram.h>

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_image.h>
#include <vcl_iostream.h>

int
main(int argc, char** argv) {
  if (argc < 2)
  {
    vcl_cerr << "Syntax: example_histogram file_in\n";
    return 1;
  }

  // The input image:
  vil1_image in = vil1_load(argv[1]);

  // The filter:
  vcl_vector<unsigned int> out = vepl_histogram(in);

  // Write output:
  for (unsigned int i=0; i<out.size(); ++i) if (out[i] != 0)
    vcl_cout << i << ": " << out[i] << vcl_endl;

  return 0;
}
