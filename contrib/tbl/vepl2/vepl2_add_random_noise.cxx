// This is tbl/vepl2/vepl2_add_random_noise.cxx
#include "vepl2_add_random_noise.h"
//:
// \file

#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_add_random_noise.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_plane.h>
#include <vxl_config.h> // for vxl_byte

vil_image_view_base_sptr vepl2_add_random_noise(vil_image_view_base const& image, double maxdev)
{
  // multi-planar image
  // since vipl does not know the concept of planes, run filter on each plane
  if (image.nplanes() > 1) {
    if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
      vil_image_view<vxl_byte>* out = new vil_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
      vil_image_view<vxl_byte> in = image, in1 = vil_plane(in,0), out1 = vil_plane(*out,0);
      vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,maxdev);
      op.put_in_data_ptr(&in1); op.put_out_data_ptr(&out1); op.filter();
      in1 = vil_plane(in,1), out1 = vil_plane(*out,1);
      op.put_in_data_ptr(&in1); op.put_out_data_ptr(&out1); op.filter();
      in1 = vil_plane(in,2), out1 = vil_plane(*out,2);
      op.put_in_data_ptr(&in1); op.put_out_data_ptr(&out1); op.filter();
      return out;
    }
    else {
      vcl_cerr << __FILE__ ": vepl2_dilate_disk() not implemented for multi-planar " << image.is_a() << '\n';
      return 0;
    }
  }

  // byte greyscale
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte>* out = new vil_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    vil_image_view<vxl_byte> in = image; // in will have 3 planes but 1 component
    vil_image_view<vxl_byte>* out = new vil_image_view<vxl_byte>(image.ni(),image.nj(),3*image.nplanes());
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<unsigned short>* out = new vil_image_view<unsigned short>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,unsigned short,unsigned short> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float>* out = new vil_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,float,float> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double>* out = new vil_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,double,double> op(GAUSSIAN_NOISE,maxdev);
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

