// .NAME example_dilate_disk
// .SECTION Description
//  This example program shows a typical use of a morphological IP class on
//  a colour image.  The input image (argv[1]) must be 24 bit (colour), and in
//  that case is dilated (circular kernel, default 3+3 cross) to argv[2]
//  which is always a PPM file image.
//  Uses vipl_dilate_disk<section<rgbcell,2>,section<rgbcell,2>,rgbcell,rgbcell>.
//  The conversion between vil_image and the in-memory section<rgbcell,2>
//  is done explicitly.
//  Note that this requires the function max(rgbcell,rgbcell) which is provided
//  here (giving the pixel with the highest R value).
//
// .SECTION Author
//   Peter Vanroose, K.U.Leuven, ESAT/PSI, 15 nov. 1997
//
// .SECTION Modifications
//   Peter Vanroose, Aug.2000 - adapted to vxl
//
#include <section/section.h>
#include <vipl/vipl_with_section/accessors/vipl_accessors_section.h>
#include <vil/vil_rgb.h>
#include <vil/vil_pixel.h>
#include <vil/vil_memory_image_of.h>

#include <vipl/vipl_dilate_disk.h>

typedef unsigned char ubyte;
typedef vil_rgb<ubyte> rgbcell;
typedef section<rgbcell,2> img_type;

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

// Necessary for vipl_dilate_disk, and not provided by the vil_rgb class itself:
bool operator< (rgbcell const& a, rgbcell const& b) { return a.r < b.r && a.g < b.g && a.b < b.b; }

int
main(int argc, char** argv) {
  if (argc < 3) { vcl_cerr << "Syntax: example_dilate_disk file_in file_out [radius]\n"; return 1; }

  // The input image:
  vil_image in = vil_load(argv[1]);
  if (! (vil_pixel_format(in) == VIL_RGB_BYTE) ) { vcl_cerr << "Please use a colour image as input\n"; return 2; }

  // The output image:
  vil_memory_image_of<rgbcell> out(in);
  
  // The image sizes:
  int xs = in.width();
  int ys = in.height();
  
  // The radius: (default is 3+3 cross)
  float radius = (argc < 4) ? 1 : atof(argv[3]);

  img_type src(xs,ys); // in-memory 2D images
  img_type dst(xs,ys);

  // set the input image:
  in.get_section(src.buffer,0,0,xs,ys);

  // The filter:
  vipl_dilate_disk<img_type,img_type,rgbcell,rgbcell, vipl_trivial_pixeliter> op(radius);
  op.put_in_data_ptr(&src);
  op.put_out_data_ptr(&dst);
  op.filter();

  // Write output:
  out.put_section(dst.buffer,0,0,xs,ys);
  vil_save(out, argv[2], "pnm");
  vcl_cout << "Written image of type PPM to " << argv[2] << vcl_endl;

  return 0;
}
