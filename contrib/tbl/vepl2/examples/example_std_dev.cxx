//:
// \file
//  This example program shows a typical use of the vepl2_moment function on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is converted to an image where each pixel is the standard
//  deviation of a 5x5 neighbourhood of the corresponding source pixel.
//  The output is written to a PGM file image.
//  Uses vepl2_moment(), vepl2_monadic_sqr(), vepl2_monadic_sqrt(),
//  vepl2_dyadic_dif() and vepl2_convert().
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   7 October 2002, from vepl/examples
//
#include <vepl2/vepl2_convert.h>
#include <vepl2/vepl2_moment.h>
#include <vepl2/vepl2_monadic.h>
#include <vepl2/vepl2_dyadic.h>

// for I/O:
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for vcl_sqrt()

typedef unsigned char ubyte;

int
main(int argc, char** argv) {
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_vepl2_moment file_in file_out\n";
    return 1;
  }

  // The input image:
  vil2_image_view_base_sptr in = vil2_load(argv[1]);
  float dummy_f = 0;
  in = vepl2_convert(*in, dummy_f);

  // The second moment filter.  result: E(X*X).
  vil2_image_view_base_sptr out = vepl2_moment(*in, 2, 5, 5);

  // The first moment filter.  result: E(X).
  vil2_image_view_base_sptr tmp = vepl2_moment(*in, 1, 5, 5);

  // The monadic "square" point operator (input=output).  result: E(X)*E(X)
  tmp = vepl2_monadic_sqr(*tmp);

  // The dyadic "minus" point operator.  result: E(X*X) - E(X)*E(X)
  vepl2_dyadic_dif(out, *tmp);

  // The monadic "square root" point operator (input = output)
  out = vepl2_monadic_sqrt(*out);

  // vepl2_convert to ubyte and write to PGM file:
  ubyte dummy = 0;
  out = vepl2_convert(*out, dummy);
  vil2_save(*out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
