// This is tbl/vepl/vepl_moment.cxx
#include <iostream>
#include "vepl_moment.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vepl/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_moment.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_new.h>
#include <vxl_config.h> // for vxl_byte

vil_image_resource_sptr vepl_moment(const vil_image_resource_sptr& image, int order, int width, int height)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // byte rgb
  if (image->nplanes() > 1 || image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    std::cerr << __FILE__ ": vepl_moment() cannot be implemented for colour images\n";
  }

  // byte greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> in = image->get_view();
    vil_image_view<vxl_byte> out = image->get_copy_view();
    vipl_moment<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(order, width, height);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // short
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> in = image->get_view();
    vil_image_view<vxl_uint_16> out = image->get_copy_view();
    vipl_moment<vil_image_view_base,vil_image_view_base,vxl_uint_16,vxl_uint_16> op(order, width, height);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // int
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_32) {
    vil_image_view<vxl_uint_32> in = image->get_view();
    vil_image_view<vxl_uint_32> out = image->get_copy_view();
    vipl_moment<vil_image_view_base,vil_image_view_base,vxl_uint_32,vxl_uint_32> op(order, width, height);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // float
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<float> out = image->get_copy_view();
    vipl_moment<vil_image_view_base,vil_image_view_base,float,float> op(order, width, height);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<double> out = image->get_copy_view();
    vipl_moment<vil_image_view_base,vil_image_view_base,double,double> op(order, width, height);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    std::cerr << __FILE__ ": vepl_moment() not implemented for " << image << '\n';

  return img_out;
}
