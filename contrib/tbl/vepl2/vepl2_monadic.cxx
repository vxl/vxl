// This is tbl/vepl2/vepl2_monadic.cxx
#include "vepl2_monadic.h"
#include <vepl2/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_monadic.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte

float abs_float(float const& a) { return (a<0) ? -a : a; }
double abs_double(double const& a) { return vcl_fabs(a); }
float sqrt_float(float const& a) { return (a<0) ? -vcl_sqrt(a) : vcl_sqrt(a); }
double sqrt_double(double const& a) { return (a<0) ? -vcl_sqrt(a) : vcl_sqrt(a); }
vxl_byte sqr_ubyte(vxl_byte const& a) { return a*a; }
float sqr_float(float const& a) { return a*a; }
vxl_uint_16 sqr_short(vxl_uint_16 const& a) { return a*a; }
vxl_uint_32 sqr_int(vxl_uint_32 const& a) { return a*a; }
double sqr_double(double const& a) { return a*a; }
static double shift_=0.0, scale_=1.0;
vxl_byte shear_ubyte(vxl_byte const& a) { return vxl_byte((a+shift_)*scale_); }
vxl_uint_16 shear_short(vxl_uint_16 const& a) { return vxl_uint_16((a+shift_)*scale_); }
vxl_uint_32 shear_int(vxl_uint_32 const& a) { return vxl_uint_32((a+shift_)*scale_); }
float shear_float(float const& a) { return float((a+shift_)*scale_); }
double shear_double(double const& a) { return (a+shift_)*scale_; }


vil_image_resource_sptr vepl2_monadic_abs(vil_image_resource_sptr image)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // byte rgb
  if (image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    // vxl_byte is unsigned so nothing happens
    img_out->put_view(*(image->get_view()));
  }

  // byte greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    // vxl_byte is unsigned so nothing happens
    img_out->put_view(*(image->get_view()));
  }

  // short
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    // vxl_uint_16 is unsigned so nothing happens
    img_out->put_view(*(image->get_view()));
  }

  // int
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_32) {
    // vxl_uint_32 is unsigned so nothing happens
    img_out->put_view(*(image->get_view()));
  }

  // float
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<float> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,float,float> op(abs_float);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<double> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,double,double> op(abs_double);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    vcl_cerr << __FILE__ ": vepl2_monadic_abs() not implemented for " << image << '\n';

  return img_out;
}

vil_image_resource_sptr vepl2_monadic_sqrt(vil_image_resource_sptr image)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // float
  if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<float> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,float,float> op(sqrt_float);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<double> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,double,double> op(sqrt_double);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    vcl_cerr << __FILE__ ": vepl2_monadic_sqrt() not implemented for " << image << '\n';

  return img_out;
}

vil_image_resource_sptr vepl2_monadic_sqr(vil_image_resource_sptr image)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // byte greyscale
  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> in = image->get_view();
    vil_image_view<vxl_byte> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(sqr_ubyte);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // short
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> in = image->get_view();
    vil_image_view<vxl_uint_16> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_uint_16,vxl_uint_16> op(sqr_short);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // int
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_32) {
    vil_image_view<vxl_uint_32> in = image->get_view();
    vil_image_view<vxl_uint_32> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_uint_32,vxl_uint_32> op(sqr_int);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // float
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<float> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,float,float> op(sqr_float);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<double> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,double,double> op(sqr_double);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    vcl_cerr << __FILE__ ": vepl2_monadic_sqr() not implemented for " << image << '\n';

  return img_out;
}

vil_image_resource_sptr vepl2_monadic_shear(vil_image_resource_sptr image, double shift, double scale)
{
  shift_ = shift; scale_ = scale;
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // byte greyscale
  if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> in = image->get_view();
    vil_image_view<vxl_byte> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_byte,vxl_byte> op(shear_ubyte);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // short
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> in = image->get_view();
    vil_image_view<vxl_uint_16> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_uint_16,vxl_uint_16> op(shear_short);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // int
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_32) {
    vil_image_view<vxl_uint_32> in = image->get_view();
    vil_image_view<vxl_uint_32> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_uint_32,vxl_uint_32> op(shear_int);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // float
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    vil_image_view<float> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,float,float> op(shear_float);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // double
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    vil_image_view<double> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,double,double> op(shear_double);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  //
  else
    vcl_cerr << __FILE__ ": vepl2_monadic_shear() not implemented for " << image << '\n';

  return img_out;
}

