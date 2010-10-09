//:
// \file
//  This example program shows a typical use of the vepl1_moment function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is converted to an image where each pixel is the standard
//  deviation of a 5x5 neighbourhood of the corresponding source pixel.
//  The output is written to a PGM file image.
//  Uses vepl1_moment(), vepl1_monadic_sqr(), vepl1_monadic_sqrt(),
//  vepl1_dyadic_dif() and vepl1_convert().
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 May 2001, from vipl/examples
//
#include <vepl1/vepl1_convert.h>
#include <vepl1/vepl1_moment.h>
#include <vepl1/vepl1_monadic.h>
#include <vepl1/vepl1_dyadic.h>

// for I/O:
#include <vil1/vil1_load.h>
#include <vil1/vil1_save.h>
#include <vcl_iostream.h>

#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv)
{
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_vepl1_moment file_in file_out\n";
    return 1;
  }

  // The input image:
  vil1_image in = vil1_load(argv[1]);
  float dummy_f = 0;
  in = vepl1_convert(in, dummy_f);

  // The second moment filter.  result: E(X*X).
  vil1_image out = vepl1_moment(in, 2, 5, 5);

  // The first moment filter.  result: E(X).
  vil1_image tmp = vepl1_moment(in, 1, 5, 5);

  // The monadic "square" point operator (input=output).  result: E(X)*E(X)
  tmp = vepl1_monadic_sqr(tmp);

  // The dyadic "minus" point operator.  result: E(X*X) - E(X)*E(X)
  vepl1_dyadic_dif(out, tmp);

  // The monadic "square root" point operator (input = output)
  out = vepl1_monadic_sqrt(out);

  // vepl1_convert to ubyte and write to PGM file:
  vxl_byte dummy = 0;
  out = vepl1_convert(out, dummy);
  vil1_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
