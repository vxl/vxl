// This is tbl/vipl/examples/example_dilate_disk.cxx

//:
// \file
//  This example program shows a typical use of a morphological IP class on
//  a grey image.  The input image (argv[1]) must be 8 bit (grey), and in
//  that case is dilated (circular kernel, default 3+3 cross) to argv[2]
//  which is always a PGM file image.
//  Uses vipl_dilate_disk<section<ubyte,2>,section<ubyte,2>,ubyte,ubyte>.
//  The conversion between vil_image and the in-memory section<ubyte,2>
//  is done explicitly.
//
//  Note that this cannot work with colour images unless a function
//  max(rgbcell,rgbcell) is provided.
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
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vipl/vipl_dilate_disk.h>

typedef unsigned char ubyte;
typedef section<ubyte,2> img_type;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>
#include <vcl_cstdlib.h> // for atof()

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_dilate_disk file_in file_out [radius]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (! (vil_pixel_format(in) == VIL_BYTE) ) { vcl_cerr << "Please use a greyscale image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<ubyte> out(in);

  // The image sizes:
  int xs = in.width();
  int ys = in.height();

  // The radius: (default is 3+3 cross)
  float radius = (argc < 4) ? 1.0f : (float)vcl_atof(argv[3]);

  img_type src(xs,ys); // in-memory 2D images
  img_type dst(xs,ys);

  // set the input image:
  in.get_section(src.buffer,0,0,xs,ys);

  // The filter:
  vipl_dilate_disk<img_type,img_type,ubyte,ubyte, vipl_trivial_pixeliter> op(radius);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  out.put_section(dst.buffer,0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PPM to " << argv[2] << vcl_endl;

  return 0;
}

// instantiation of the filter;
// this should normally go into a separate file in the Templates subdirectory
#include <vipl/vipl_with_section/accessors/vipl_accessors_section.txx>
#include <vipl/vipl_dilate_disk.txx>

template class vipl_dilate_disk<img_type,img_type,ubyte,ubyte, vipl_trivial_pixeliter>;
