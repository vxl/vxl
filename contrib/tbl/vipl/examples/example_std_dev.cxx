// .NAME example_std_dev
// .SECTION Description
//  This example program shows a typical use of the vipl_moment IP class on
//  a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case is converted to an image where each pixel is the standard
//  deviation of a 5x5 neighbourhood of the corresponding source pixel.
//  The output is written to a PGM file image.
//  Uses vipl_moment<vil_image,vil_image,ubyte,float>, vipl_monadic<vil_image,vil_image,float,float>,
//  vipl_dyadic<vil_image,vil_image,float,float> and vipl_convert<vil_image,vil_image,float,ubyte> .
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 15 sept. 1999
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>
#include <vipl/vipl_moment.h>
#include <vipl/vipl_monadic.h>
#include <vipl/vipl_dyadic.h>
#include <vipl/vipl_convert.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h> // for vcl_sqrt()

typedef unsigned char ubyte;
float square(float const& x) { return x*x; }
void is_minus(float& x, float const& y) { x-=y; }
float squareroot(float const& x) { return vcl_sqrt(x); }

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_vipl_moment file_in file_out\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (vil_pixel_format(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<float> out (in);

  // Intermediate image:
  vil_memory_image_of<ubyte> tmp (out);

  // The second moment filter.  result: E(X*X), into out.
  vipl_moment<vil_image,vil_image,ubyte,float VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> scnd_moment(2, 5, 5);
  scnd_moment.put_in_data_ptr(&in);
  scnd_moment.put_out_data_ptr(&out);
  scnd_moment.filter();

  // The first moment filter.  result: E(X), into tmp.
  vipl_moment<vil_image,vil_image,ubyte,float VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> frst_moment(1, 5, 5);
  frst_moment.put_in_data_ptr(&in);
  frst_moment.put_out_data_ptr(&tmp);
  frst_moment.filter();

  // The monadic "square" point operator (input=output).  result: E(X)*E(X)
  vipl_monadic<vil_image,vil_image,float,float VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> square_op(square);
  square_op.put_in_data_ptr(&tmp);
  square_op.put_out_data_ptr(&tmp);
  square_op.filter();

  // The dyadic "is_minus" point operator.  result: E(X*X) - E(X)*E(X)
  vipl_dyadic<vil_image,vil_image,float,float VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> minus_op(is_minus);
  minus_op.put_in_data_ptr(&tmp);
  minus_op.put_out_data_ptr(&out);
  minus_op.filter();

  // The monadic "square root" point operator (input = output)
  vipl_monadic<vil_image,vil_image,float,float VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> sqrt_op(squareroot);
  sqrt_op.put_in_data_ptr(&out);
  sqrt_op.put_out_data_ptr(&out);
  sqrt_op.filter();

  // vipl_convert to ubyte and write to PGM file:
  vil_memory_image_of<ubyte> pgm(in);
  vipl_convert<vil_image,vil_image,float,ubyte VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
  op.put_in_data_ptr(&out);
  op.put_out_data_ptr(&pgm);
  op.filter();
  vil_save(pgm, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
