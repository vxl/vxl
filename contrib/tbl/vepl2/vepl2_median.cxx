// This is tbl/vepl2/vepl2_median.cxx
#include "vepl2_median.h"
#include <vcl_iostream.h>
#include <vepl2/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_median.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_plane.h>
#include <vxl_config.h> // for vxl_byte

vil_image_view_base_sptr vepl2_median(vil_image_view_base const& image, float radius)
{
  // multi-planar image
  // since vipl does not know the concept of planes, run filter on each plane
  if (image.nplanes() > 1) {
    if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
      vil_image_view<vxl_byte>* out = new vil_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
      vil_image_view<vxl_byte> in = image, in1 = vil_plane(in,0), out1 = vil_plane(*out,0);
      vipl_median<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(radius);
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
    vipl_median<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    vil_image_view<vxl_byte> in = image; // in will have 3 planes but 1 component
    vil_image_view<vxl_byte>* out = new vil_image_view<vxl_byte>(image.ni(),image.nj(),3*image.nplanes());
    vipl_median<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(radius);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16>* out = new vil_image_view<vxl_uint_16>(image.ni(),image.nj(),image.nplanes());
    vipl_median<vil_image_view_base,vil_image_view_base,vxl_uint_16,vxl_uint_16> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // int
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_UINT_32) {
    vil_image_view<vxl_uint_32>* out = new vil_image_view<vxl_uint_32>(image.ni(),image.nj(),image.nplanes());
    vipl_median<vil_image_view_base,vil_image_view_base,vxl_uint_32,vxl_uint_32> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float>* out = new vil_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_median<vil_image_view_base,vil_image_view_base,float,float> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double>* out = new vil_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_median<vil_image_view_base,vil_image_view_base,double,double> op(radius);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_median() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

