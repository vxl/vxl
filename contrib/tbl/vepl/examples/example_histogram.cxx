//:
// \file
//  This example program shows a typical use of the vepl_histogram function on
//  an int image.  The input image (argv[1]) can be any scalar type (i.e.,
//  it should be mappable to int: [[un]signed] char, [unsigned]short, int),
//  and its histogram is calculated and written to stdout.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/examples
//
#include <iostream>
#include <vepl/vepl_histogram.h>

// for I/O:
#include <vil/vil_load.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

int
main(int argc, char** argv) {
  if (argc < 2)
  {
    std::cerr << "Syntax: example_histogram file_in\n";
    return 1;
  }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]);
  if (!in) { std::cerr << "Please use a ubyte image as input\n"; return 2; }

  // The filter:
  std::vector<unsigned int> out = vepl_histogram(in);

  // Write output:
  for (unsigned int i=0; i<out.size(); ++i) if (out[i] != 0)
    std::cout << i << ": " << out[i] << std::endl;

  return 0;
}
