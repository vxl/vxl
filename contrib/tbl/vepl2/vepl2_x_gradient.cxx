// This is tbl/vepl2/vepl2_x_gradient.cxx
#include "vepl2_x_gradient.h"
#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_x_gradient.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_rgb.h>
#include <vxl_config.h> // for vxl_byte

vil2_image_view_base_sptr vepl2_x_gradient(vil2_image_view_base const& image, double scale, double shift)
{
  // byte rgb
  if (image.pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE) {
    typedef vil2_rgb<vxl_byte> r_g_b;
    vil2_image_view<r_g_b>* out = new vil2_image_view<r_g_b>(image.ni(),image.nj(),image.nplanes());
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,r_g_b,r_g_b,vipl_trivial_pixeliter>
      op(scale, r_g_b((vxl_byte)(shift+0.5)) );
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (image.nplanes() == 3 && image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    typedef vil2_rgb<vxl_byte> r_g_b;
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),3);
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,r_g_b,r_g_b,vipl_trivial_pixeliter>
      op(scale, r_g_b((vxl_byte)(shift+0.5)) );
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // byte greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte,vipl_trivial_pixeliter>
      op(scale, (vxl_byte)(shift+0.5));
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    vil2_image_view<vxl_uint_16>* out = new vil2_image_view<vxl_uint_16>(image.ni(),image.nj(),image.nplanes());
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,vxl_uint_16,vxl_uint_16,vipl_trivial_pixeliter>
      op(scale, (vxl_uint_16)(shift+0.5));
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // int
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32) {
    vil2_image_view<vxl_uint_32>* out = new vil2_image_view<vxl_uint_32>(image.ni(),image.nj(),image.nplanes());
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,vxl_uint_32,vxl_uint_32,vipl_trivial_pixeliter>
      op(scale, (vxl_uint_32)(shift+0.5));
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,float,float,vipl_trivial_pixeliter>
      op(scale, (float)shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_x_gradient<vil2_image_view_base,vil2_image_view_base,double,double,vipl_trivial_pixeliter>
      op(scale, shift);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_x_gradient() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

