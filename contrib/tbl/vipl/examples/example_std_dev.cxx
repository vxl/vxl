//:
// \file
//  This example program shows a typical use of the vipl_moment IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is converted to an image where each pixel is the standard
//  deviation of a 5x5 neighbourhood of the corresponding source pixel.
//  The output is written to a PGM file image.
//  Uses vipl_moment<vil_image_view<ubyte>,vil_image_view<ubyte>,ubyte,float>,
//  vipl_monadic<vil_image_view<ubyte>,vil_image_view<ubyte>,float,float>,
//  vipl_dyadic<vil_image_view<ubyte>,vil_image_view<ubyte>,float,float> and
//  vipl_convert<vil_image_view<ubyte>,vil_image_view<ubyte>,float,ubyte> .
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 sept. 1999
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim
//
#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vil/vil_image_view.h>
#include <vipl/vipl_moment.h>
#include <vipl/vipl_monadic.h>
#include <vipl/vipl_dyadic.h>
#include <vipl/vipl_convert.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for vcl_sqrt()

#include <vxl_config.h> // for vxl_byte
float square(float const& x) { return x*x; }
void is_minus(float& x, float const& y) { x-=y; }
float squareroot(float const& x) { return vcl_sqrt(x); }

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_vipl_moment file_in file_out\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_image_view<float> out(in.ni(),in.nj(),in.nplanes());

  // Intermediate image:
  vil_image_view<float> tmp(in.ni(),in.nj(),in.nplanes());

  // The second moment filter.  result: E(X*X), into out.
  vipl_moment<vil_image_view<vxl_byte>,vil_image_view<float>,vxl_byte,float> scnd_moment(2,5,5);
  scnd_moment.put_in_data_ptr(&in);
  scnd_moment.put_out_data_ptr(&out);
  scnd_moment.filter();

  // The first moment filter.  result: E(X), into tmp.
  vipl_moment<vil_image_view<vxl_byte>,vil_image_view<float>,vxl_byte,float> frst_moment(1,5,5);
  frst_moment.put_in_data_ptr(&in);
  frst_moment.put_out_data_ptr(&tmp);
  frst_moment.filter();

  // The monadic "square" point operator (input=output).  result: E(X)*E(X)
  vipl_monadic<vil_image_view<float>,vil_image_view<float>,float,float> square_op(square);
  square_op.put_in_data_ptr(&tmp);
  square_op.put_out_data_ptr(&tmp);
  square_op.filter();

  // The dyadic "is_minus" point operator.  result: E(X*X) - E(X)*E(X)
  vipl_dyadic<vil_image_view<float>,vil_image_view<float>,float,float> minus_op(is_minus);
  minus_op.put_in_data_ptr(&tmp);
  minus_op.put_out_data_ptr(&out);
  minus_op.filter();

  // The monadic "square root" point operator (input = output)
  vipl_monadic<vil_image_view<float>,vil_image_view<float>,float,float> sqrt_op(squareroot);
  sqrt_op.put_in_data_ptr(&out);
  sqrt_op.put_out_data_ptr(&out);
  sqrt_op.filter();

  // vipl_convert to vxl_byte and write to PGM file:
  vil_image_view<vxl_byte> pgm(in.ni(),in.nj(),in.nplanes());
  vipl_convert<vil_image_view<float>,vil_image_view<vxl_byte>,float,vxl_byte> op;
  op.put_in_data_ptr(&out);
  op.put_out_data_ptr(&pgm);
  op.filter();
  vil_save(pgm, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
