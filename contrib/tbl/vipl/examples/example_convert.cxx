//:
// \file
//  This example program shows a typical use of the vipl_convert IP class on
//  any image.  The input image (argv[1]) is converted to a ubyte image
//  and written to argv[2] which is always a PGM file image.
//  When the input image is RGB, its intensity is extracted.
//  To this end an explicit converter from vil_rgb<ubyte> to ubyte is necessary.
//  When it is short, int or float, an implicit mapping is done to 0--255.
//  Uses vipl_convert<vil_image_view<T>,vil_image_view<T>,T,ubyte>.
//
// \author Peter Vanroose, K.U.Leuven, ESAT/PSI
// \date   29 may 1998
//
// \verbatim
// Modifications:
//   Peter Vanroose, Aug.2000 - adapted to vxl
//   Peter Vanroose, Feb.2004 - replaced vil1_image by vil2_image_view<T>
// \endverbatim
//
#include <vil/vil_rgb.h>
#include <vil/vil_image_view.h>
#include <vipl/accessors/vipl_accessors_vil_image_view.h>
#include <vipl/vipl_convert.h>

// for I/O:
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>

#include <vxl_config.h> // for vxl_byte

int
main(int argc, char** argv)
{
  if (argc < 3) { vcl_cerr << "Syntax: example_convert file_in file_out\n"; return 1; }

  // The input image:
  vil_image_view_base_sptr in = vil_load(argv[1]);

  // The output image:
  vil_image_view<vxl_byte> out(in->ni(),in->nj(),in->nplanes());

  if (in->nplanes() == 3) { // colour (RGB)
    if (in->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
      vipl_convert<vil_image_view<vxl_byte>,vil_image_view<vxl_byte>,vxl_byte,vxl_byte> op;
      vil_image_view<vil_rgb<vxl_byte> > tmp = in; vil_image_view<vxl_byte> tmp2 = tmp;
      op.put_in_data_ptr(&tmp2); op.put_out_data_ptr(&out); op.filter();
      vcl_cout << "vipl_converted ubyte RGB image to PGM image " << argv[2] << vcl_endl;
    }
    else {
      vcl_cerr<<"Not yet implemented conversion for non-8-bit colour pixels\n";
      return 1;
    }
  }
  else if (in->nplanes() != 1) { // not monochrome nore RGB
    vcl_cerr << "Cannot currently convert image with "<< in->nplanes() <<" planes\n";
    return 1;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_COMPLEX_FLOAT ||
           in->pixel_format() == VIL_PIXEL_FORMAT_COMPLEX_DOUBLE) {
    vcl_cerr << "Cannot currently convert complex pixel type images\n";
    return 1;
  }
  else if (vil_pixel_format_component_format(in->pixel_format()) != in->pixel_format()) { // RGB or RGBA
    if (in->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
      vipl_convert<vil_image_view<vxl_byte>,vil_image_view<vxl_byte>,vxl_byte,vxl_byte> op;
      vil_image_view<vil_rgb<vxl_byte> > tmp = in; vil_image_view<vxl_byte> tmp2 = tmp;
      op.put_in_data_ptr(&tmp2); op.put_out_data_ptr(&out); op.filter();
      vcl_cout << "vipl_converted ubyte RGB image to PGM image " << argv[2] << vcl_endl;
    }
    else {
      vcl_cerr<<"Not yet implemented conversion for non-8-bit colour pixels\n";
      return 1;
    }
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    // no conversion necessary
    out = in;
    vcl_cout << "vipl_converted ubyte image to PGM image "<< argv[2] <<" (no conversion was necessary)\n";
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_SBYTE)
  {
    vipl_convert<vil_image_view<vxl_sbyte>,vil_image_view<vxl_byte>,vxl_sbyte,vxl_byte> op;
    vil_image_view<vxl_sbyte> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted sbyte image to PGM image "<< argv[2] << vcl_endl;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vipl_convert<vil_image_view<vxl_uint_16>,vil_image_view<vxl_byte>,vxl_uint_16,vxl_byte> op;
    vil_image_view<vxl_uint_16> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted uint_16 image to PGM image "<< argv[2] << vcl_endl;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_UINT_16)
  {
    vipl_convert<vil_image_view<vxl_int_16>,vil_image_view<vxl_byte>,vxl_int_16,vxl_byte> op;
    vil_image_view<vxl_int_16> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted int_16 image to PGM image "<< argv[2] << vcl_endl;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_UINT_32)
  {
    vipl_convert<vil_image_view<vxl_uint_32>,vil_image_view<vxl_byte>,vxl_uint_32,vxl_byte> op;
    vil_image_view<vxl_uint_32> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted uint_32 image to PGM image "<< argv[2] << vcl_endl;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_UINT_32)
  {
    vipl_convert<vil_image_view<vxl_int_32>,vil_image_view<vxl_byte>,vxl_int_32,vxl_byte> op;
    vil_image_view<vxl_int_32> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted int_32 image to PGM image "<< argv[2] << vcl_endl;
  }
#if VXL_HAS_INT_64
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_UINT_64)
  {
    vipl_convert<vil_image_view<vxl_uint_64>,vil_image_view<vxl_byte>,vxl_uint_64,vxl_byte> op;
    vil_image_view<vxl_uint_64> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted uint_64 image to PGM image "<< argv[2] << vcl_endl;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_UINT_64)
  {
    vipl_convert<vil_image_view<vxl_int_64>,vil_image_view<vxl_byte>,vxl_int_64,vxl_byte> op;
    vil_image_view<vxl_int_64> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted int_64 image to PGM image "<< argv[2] << vcl_endl;
  }
#endif
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_FLOAT)
  {
    vipl_convert<vil_image_view<float>,vil_image_view<vxl_byte>,float,vxl_byte> op;
    vil_image_view<float> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted float image to PGM image "<< argv[2] << vcl_endl;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE)
  {
    vipl_convert<vil_image_view<double>,vil_image_view<vxl_byte>,double,vxl_byte> op;
    vil_image_view<double> tmp = in;
    op.put_in_data_ptr(&tmp); op.put_out_data_ptr(&out); op.filter();
    vcl_cout << "vipl_converted double image to PGM image "<< argv[2]<<vcl_endl;
  }
#if 0
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_PYRAMID) {
    vcl_cerr << "Cannot currently convert pyramid images\n";
    return 1;
  }
  else if (in->pixel_format() == VIL_PIXEL_FORMAT_BANDED) {
    vcl_cerr << "Cannot currently convert banded images\n";
    return 1;
  }
#endif
  else {
    vcl_cerr << "Never heared of image format "<< int(in->pixel_format())<<'\n';
    return 1;
  }

  vil_save(out, argv[2], "pnm");
  return 0;
}
