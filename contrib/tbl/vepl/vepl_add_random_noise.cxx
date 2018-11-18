// This is tbl/vepl/vepl_add_random_noise.cxx
#include "vepl_add_random_noise.h"
//:
// \file

#include <vepl/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_add_random_noise.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_plane.h>
#include <vil/vil_new.h>
#include <vxl_config.h> // for vxl_byte

vil_image_resource_sptr vepl_add_random_noise(const vil_image_resource_sptr& image, double maxdev)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // multi-planar image
  // since vipl does not know the concept of planes, run filter on each plane
  if (image->nplanes() > 1) {
    if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
      vil_image_view<vxl_byte> in = image->get_view();
      vil_image_view<vxl_byte> out = image->get_copy_view();
      vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,maxdev);
      for (unsigned int p=0; p<image->nplanes(); ++p) {
        vil_image_view<vxl_byte> i = vil_plane(in,p), o = vil_plane(out,p);
        op.put_in_data_ptr(&i); op.put_out_data_ptr(&o); op.filter();
      }
      img_out->put_view(out);
    }
    else
      std::cerr << __FILE__ ": vepl_dilate_disk() not implemented for multi-planar " << image << '\n';
  }

  // byte greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> in = image->get_view();
    vil_image_view<vxl_byte> out = image->get_copy_view();
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // byte rgb
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    img_out = vil_new_image_resource(image->ni(), image->nj()*image->nplanes(), 1, image->pixel_format());
    vil_image_view<vxl_byte> in = image->get_view(); // in will have 3 planes but 1 component
    vil_image_view<vxl_byte> out = image->get_copy_view();
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // short
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> in = image->get_view();
    vil_image_view<vxl_uint_16> out = image->get_copy_view();
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,vxl_uint_16,vxl_uint_16> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // float
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<float> out = image->get_copy_view();
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,float,float> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<double> out = image->get_copy_view();
    vipl_add_random_noise<vil_image_view_base,vil_image_view_base,double,double> op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    std::cerr << __FILE__ ": vepl_add_random_noise() not implemented for " << image << '\n';

  return img_out;
}
