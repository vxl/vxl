// This is tbl/vepl2/vepl2_threshold.cxx
#include "vepl2_threshold.h"
#include <vcl_iostream.h>
#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_threshold.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vxl_config.h> // for vxl_byte

vil2_image_view_base_sptr vepl2_threshold(vil2_image_view_base const& image, double threshold, double below, double above)
{
  // byte rgb
  if (image.nplanes() > 1 || image.pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE) {
    vcl_cerr << __FILE__ ": vepl2_threshold() cannot be implemented for colour images\n";
    return 0;
  }

  // byte greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_threshold<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte,vipl_trivial_pixeliter>
      op((vxl_byte)threshold, (vxl_byte)below, (vxl_byte)above);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    typedef unsigned short ushort;
    vil2_image_view<ushort>* out = new vil2_image_view<ushort>(image.ni(),image.nj(),image.nplanes());
    vipl_threshold<vil2_image_view_base,vil2_image_view_base,ushort,ushort,vipl_trivial_pixeliter>
      op((ushort)threshold, (ushort)below, (ushort)above);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_threshold<vil2_image_view_base,vil2_image_view_base,float,float,vipl_trivial_pixeliter>
      op((float)threshold, (float)below, (float)above);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_threshold<vil2_image_view_base,vil2_image_view_base,double,double,vipl_trivial_pixeliter>
      op(threshold, below, above);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_threshold() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

