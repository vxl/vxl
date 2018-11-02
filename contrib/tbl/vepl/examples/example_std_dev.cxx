//:
// \file
//  This example program shows a typical use of the vepl_moment function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is converted to an image where each pixel is the standard
//  deviation of a 5x5 neighbourhood of the corresponding source pixel.
//  The output is written to a PGM file image.
//  Uses vepl_moment(), vepl_monadic_sqr(), vepl_monadic_sqrt(),
//  vepl_dyadic_dif() and vepl_convert().
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl1/examples
//
#include <iostream>
#include <vepl/vepl_convert.h>
#include <vepl/vepl_moment.h>
#include <vepl/vepl_monadic.h>
#include <vepl/vepl_dyadic.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Syntax: example_vepl_moment file_in file_out\n";
    return 1;
  }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]); // any type
  in = vepl_convert(in, 0.f); // dummy second argument ==> conversion to float

  // The second moment filter.  result: E(X*X).
  vil_image_resource_sptr out = vepl_moment(in, 2, 5, 5);

  // The first moment filter.  result: E(X).
  vil_image_resource_sptr tmp = vepl_moment(in, 1, 5, 5);

  // The monadic "square" point operator (input=output).  result: E(X)*E(X)
  tmp = vepl_monadic_sqr(tmp);

  // The dyadic "minus" point operator.  result: E(X*X) - E(X)*E(X)
  vepl_dyadic_dif(out, tmp);

  // The monadic "square root" point operator (input = output)
  out = vepl_monadic_sqrt(out);

  // vepl_convert to ubyte and write to PGM file:
  out = vepl_convert(out, (vxl_byte)0);
  if (vil_save_image_resource(out, argv[2], "pnm"))
    std::cout << "Written standard-deviation image to PNM image "<< argv[2]<< '\n';
  else
    std::cout << "Could not write standard-deviation image as PNM to " << argv[2] << '\n';

  return 0;
}
