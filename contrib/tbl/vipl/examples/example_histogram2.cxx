//:
// \file
//  This example program shows a typical use of the vipl_histogram IP class on
//  an int image.  The input image (argv[1]) can be any scalar type (i.e.,
//  it should be mappable to int: [[un]signed] char, [unsigned]short, int),
//  and its histogram is calculated and written to stdout.
//  Uses vipl_histogram<section<ubyte,2>,section<int,2>,ubyte,int>.
//  The conversion between vil_image and the in-memory section<ubyte,2>
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
// \endverbatim
//
#include <section/section.h>
#include <vipl/vipl_with_section/accessors/vipl_accessors_section.h>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vil/vil_pixel.h>
#include <vipl/vipl_histogram.h>
#include <vcl_vector.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_image.h>
#include <vcl_iostream.h>

#ifndef VCL_VC50
typedef unsigned char ubyte;
#else
typedef int ubyte; // this is a hack!!!  See the Description.
#endif

int
main(int argc, char** argv) {
  if (argc < 2) { vcl_cerr << "Syntax: example_histogram file_in\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  vcl_vector<unsigned> out(256);

  section<ubyte,2> src(in.width(),in.height()); // in-memory 2D image

  // set the input image:
  if (vil_pixel_format(in) != VIL_BYTE) { vcl_cerr << "Please use a ubyte image as input\n"; return 2; }
  in.get_section(src.buffer,0,0,in.width(),in.height());

  // The filter:
  vipl_histogram<section<ubyte,2>, 
    vcl_vector<unsigned>,
    unsigned char,
    unsigned VCL_DFL_TMPL_ARG(vipl_trivial_pixeliter)> op;
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&out);
  op.filter();

  // Write output:
  {for (int i=0; i<256; ++i) if (out[i] != 0)
     vcl_cout << i << ": " << int(out[i]) << vcl_endl;
  }

  return 0;
}
