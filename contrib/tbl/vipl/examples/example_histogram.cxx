//:
// \file
//  This example program shows a typical use of the vipl_histogram IP class on
//  an int image.  The input image (argv[1]) can be any scalar type (i.e.,
//  it should be mappable to int: [[un]signed] char, [unsigned]short, int),
//  and its histogram is calculated and written to stdout.
//  Uses vipl_histogram<section<ubyte,2>,section<int,2>,ubyte,int>.
//  The conversion between vil_image_view<ubyte> and the in-memory section<ubyte,2>
//  is done explicitly, pixel per pixel (because of possibly different types).
//
//  Note that it seems to be impossible with some compilers (notably
//  VisualC++ 5.0) to specify different types for input and output images!
//  That explains why I use section<int,2> and not section<ubyte,2>
//  as input image type.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   15 nov. 1997
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_load by vil2_load
// \endverbatim
//
#include <section/section.h>
#include <vipl/vipl_with_section/accessors/vipl_accessors_section.h>
#include <vipl/vipl_histogram.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vcl_iostream.h>
#include <vcl_cstring.h> // for memcpy()

#include <vxl_config.h> // for vxl_byte

#ifdef VCL_VC50
#define vxl_byte int // this is a hack!!!  See the Description.
#endif

int
main(int argc, char** argv)
{
  if (argc < 2) { vcl_cerr << "Syntax: example_histogram file_in\n"; return 1; }

  // The input image:
  vil_image_view<vxl_byte> in = vil_load(argv[1]);

  section<vxl_byte,2> src(in.ni(),in.nj()); // in-memory 2D image
  section<int,2> dst(1,256);

  // set the input image:
  if (!in) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  vcl_memcpy(src.buffer, in.memory_chunk()->const_data(), in.size_bytes());

  // The filter:
  vipl_histogram<section<vxl_byte,2>,section<int,2>,vxl_byte,int> op;
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  {for (int i=0; i<256; ++i) if (src.buffer[i] != 0)
     vcl_cout << i << ": " << int(src.buffer[i]) << vcl_endl;
  }

  return 0;
}
