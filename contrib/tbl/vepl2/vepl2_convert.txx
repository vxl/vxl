// This is tbl/vepl2/vepl2_convert.txx
#ifndef vepl2_convert_txx_
#define vepl2_convert_txx_

#include "vepl2_convert.h"
#include <vepl2/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_convert.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_rgb.h>
#include <vxl_config.h> // for vxl_byte

template <class D>
vil_image_view_base_sptr vepl2_convert(vil_image_view_base const& image, D dummy)
{
  // byte greyscale
  if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<D > out(image.ni(),image.nj(),image.nplanes());
    vipl_convert<vil_image_view_base,vil_image_view_base,vxl_byte,D> op;
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(&out);
    op.filter();
    return &out;
  }

  // byte rgb
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    vil_image_view<D > out(image.ni(),image.nj(),image.nplanes());
    vipl_convert<vil_image_view_base,vil_image_view_base,vil_rgb<vxl_byte>,D> op;
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(&out);
    op.filter();
    return &out;
  }

  // float
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<D > out(image.ni(),image.nj(),image.nplanes());
    vipl_convert<vil_image_view_base,vil_image_view_base,float,D> op;
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(&out);
    op.filter();
    return &out;
  }

  // double
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<D > out(image.ni(),image.nj(),image.nplanes());
    vipl_convert<vil_image_view_base,vil_image_view_base,double,D> op;
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(&out);
    op.filter();
    return &out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_convert() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

#endif // vepl2_convert_txx_
