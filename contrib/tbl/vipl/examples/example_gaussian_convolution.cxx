// This is tbl/vipl/examples/example_gaussian_convolution.cxx

//:
// \file
//  This example program shows a typical use of the gaussian convolution filter
//  on a ubyte image.  The input image (argv[1]) must be ubyte, and in that
//  case its convolved with a gaussian kernel and the result is written to
//  argv[2] which is always a PGM file image.
//  Uses vipl_gaussian_convolution<vnl_matrix<ubyte>,vnl_matrix<ubyte>,ubyte,ubyte>.
//  The conversion between vil_image_view<ubyte> and the in-memory vnl_matrix<ubyte>
//  is done explicitly.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   April 2002
//
// \verbatim
// Modifications:
//   Peter Vanroose, Feb.2004 - replaced mil_image by vil2_image_view<T>
//   Peter Vanroose, Feb.2004 - replaced vil1_load by vil2_load
// \endverbatim
//
#include <vnl/vnl_matrix.h>

#include <vipl/vipl_with_vnl_matrix/accessors/vipl_accessors_vnl_matrix.h>
#include <vipl/vipl_gaussian_convolution.h>

#include <vxl_config.h> // for vxl_byte

typedef vnl_matrix<vxl_byte> img_type;

// for I/O:
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()
#include <vcl_cstring.h> // for memcpy()

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_gaussian_convolution file_in file_out [sigma]\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }

  // The output image:
  vil_image_view<vxl_byte> out(in.ni(),in.nj(),in.nplanes());

  // The image sizes:
  int xs = in.ni();
  int ys = in.nj();

  // The value of sigma: (default is 5)
  float sigma = (argc < 4) ? 5.0f : (float)vcl_atof(argv[3]);

  img_type src(ys,xs);
  img_type dst(ys,xs);

  // set the input image:
  vcl_memcpy(src.begin(), in.memory_chunk()->const_data(), in.size_bytes());

  // The filter:
  vipl_gaussian_convolution<img_type,img_type,vxl_byte,vxl_byte> op(sigma);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  vcl_memcpy(out.memory_chunk()->data(), dst.begin(), out.size_bytes());
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PGM to " << argv[2] << vcl_endl;

  return 0;
}
