// This is tbl/vepl2/vepl2_add_random_noise.cxx
#include "vepl2_add_random_noise.h"
//:
// \file

#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_add_random_noise.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_rgb.h>
#include <vxl_config.h> // for vxl_byte

vil2_image_view_base_sptr vepl2_add_random_noise(vil2_image_view_base const& image, double maxdev)
{
  // byte rgb
  if (image.pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE)
  {
    vil2_image_view<vil2_rgb<vxl_byte> >* out = new vil2_image_view<vil2_rgb<vxl_byte> >(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (image.nplanes() == 3 && image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),3);
    vipl_add_random_noise<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // byte greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil2_image_view_base,vil2_image_view_base,float,float>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil2_image_view_base,vil2_image_view_base,double,double>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_add_random_noise() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

