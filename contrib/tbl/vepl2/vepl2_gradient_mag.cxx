// This is tbl/vepl2/vepl2_gradient_mag.cxx
#include "vepl2_gradient_mag.h"
#include <vcl_iostream.h>
#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_gradient_mag.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vxl_config.h> // for vxl_byte

vil2_image_view_base_sptr vepl2_gradient_mag(vil2_image_view_base const& image, double scale, double shift)
{
  // byte rgb
  if (image.nplanes() > 1 || image.pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE) {
    vcl_cerr << __FILE__ ": vepl2_gradient_mag() cannot be implemented for colour images\n";
    return 0;
  }

  // byte greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_gradient_mag<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte,vipl_trivial_pixeliter> op(scale, shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    vil2_image_view<vxl_uint_16>* out = new vil2_image_view<vxl_uint_16>(image.ni(),image.nj(),image.nplanes());
    vipl_gradient_mag<vil2_image_view_base,vil2_image_view_base,vxl_uint_16,vxl_uint_16,vipl_trivial_pixeliter> op(scale, shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // int
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32) {
    vil2_image_view<vxl_uint_32>* out = new vil2_image_view<vxl_uint_32>(image.ni(),image.nj(),image.nplanes());
    vipl_gradient_mag<vil2_image_view_base,vil2_image_view_base,vxl_uint_32,vxl_uint_32,vipl_trivial_pixeliter> op(scale, shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_gradient_mag<vil2_image_view_base,vil2_image_view_base,float,float,vipl_trivial_pixeliter> op(scale, shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_gradient_mag<vil2_image_view_base,vil2_image_view_base,double,double,vipl_trivial_pixeliter> op(scale, shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_gradient_mag() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

