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
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv)
{
  if (argc < 3)
  {
    vcl_cerr << "Syntax: example_vepl2_moment file_in file_out\n";
    return 1;
  }

  // The input image:
  vil_image_resource_sptr in = vil_load_image_resource(argv[1]); // any type
  in = vepl2_convert(in, 0.f); // dummy second argument ==> conversion to float

  // The second moment filter.  result: E(X*X).
  vil_image_resource_sptr out = vepl2_moment(in, 2, 5, 5);

  // The first moment filter.  result: E(X).
  vil_image_resource_sptr tmp = vepl2_moment(in, 1, 5, 5);

  // The monadic "square" point operator (input=output).  result: E(X)*E(X)
  tmp = vepl2_monadic_sqr(tmp);

  // The dyadic "minus" point operator.  result: E(X*X) - E(X)*E(X)
  vepl2_dyadic_dif(out, tmp);

  // The monadic "square root" point operator (input = output)
  out = vepl2_monadic_sqrt(out);

  // vepl2_convert to ubyte and write to PGM file:
  out = vepl2_convert(out, (vxl_byte)0);
  vil_save(*(out->get_view()), argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
