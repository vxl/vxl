// This is tbl/vepl/vepl_convert.hxx
#ifndef vepl_convert_hxx_
#define vepl_convert_hxx_

#include "vepl_convert.h"
#include <vepl/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_new.h>
#include <vxl_config.h> // for vxl_byte

template <class D>
vil_image_resource_sptr vepl_convert(vil_image_resource_sptr image, D /*dummy*/)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // byte greyscale
  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> in = image->get_view();
    vil_image_view<D> out = image->get_copy_view();
    vipl_convert<vil_image_view_base,vil_image_view_base,vxl_byte,D> op;
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // float
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<D > out(image->ni(),image->nj(),image->nplanes());
    vipl_convert<vil_image_view_base,vil_image_view_base,float,D> op;
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<D > out(image->ni(),image->nj(),image->nplanes());
    vipl_convert<vil_image_view_base,vil_image_view_base,double,D> op;
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    std::cerr << __FILE__ ": vepl_convert() not implemented for " << image << '\n';

  return img_out;
}

#endif // vepl_convert_hxx_
