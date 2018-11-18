// This is tbl/vepl/vepl_monadic.cxx
#include <iostream>
#include <cmath>
#include "vepl_monadic.h"
#include <vepl/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_monadic.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte

vxl_sbyte abs_byte(vxl_sbyte const& a) { return (a<0) ? -a : a; }
vxl_int_16 abs_short(vxl_int_16 const& a) { return (a<0) ? -a : a; }
vxl_int_32 abs_int(vxl_int_32 const& a) { return (a<0) ? -a : a; }
float abs_float(float const& a) { return (a<0) ? -a : a; }
double abs_double(double const& a) { return std::fabs(a); }
float sqrt_float(float const& a) { return (a<0) ? -std::sqrt(-a) : std::sqrt(a); }
double sqrt_double(double const& a) { return (a<0) ? -std::sqrt(-a) : std::sqrt(a); }
vxl_byte sqr_ubyte(vxl_byte const& a) { return vxl_byte(a*a); }
float sqr_float(float const& a) { return a*a; }
vxl_uint_16 sqr_short(vxl_uint_16 const& a) { vxl_uint_32 b = a; return vxl_uint_16(b*b); }
vxl_uint_32 sqr_int(vxl_uint_32 const& a) { return a*a; }
double sqr_double(double const& a) { return a*a; }
static double shift_=0.0, scale_=1.0;
vxl_byte shear_ubyte(vxl_byte const& a) { return vxl_byte((a+shift_)*scale_); }
vxl_uint_16 shear_short(vxl_uint_16 const& a) { return vxl_uint_16((a+shift_)*scale_); }
vxl_uint_32 shear_int(vxl_uint_32 const& a) { return vxl_uint_32((a+shift_)*scale_); }
float shear_float(float const& a) { return float((a+shift_)*scale_); }
double shear_double(double const& a) { return (a+shift_)*scale_; }


vil_image_resource_sptr vepl_monadic_abs(const vil_image_resource_sptr& image)
{
  vil_image_resource_sptr img_out = vil_new_image_resource(image->ni(), image->nj(), image->nplanes(), image->pixel_format());

  // first the unsigned types
  //
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

  // then the signed types
  //
  // byte greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_SBYTE) {
    vil_image_view<vxl_sbyte> in = image->get_view();
    vil_image_view<vxl_sbyte> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_sbyte,vxl_sbyte> op(abs_byte);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // short int greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_INT_16) {
    vil_image_view<vxl_int_16> in = image->get_view();
    vil_image_view<vxl_int_16> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_int_16,vxl_int_16> op(abs_short);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // int greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_INT_32) {
    vil_image_view<vxl_int_32> in = image->get_view();
    vil_image_view<vxl_int_32> out = image->get_copy_view();
    vipl_monadic<vil_image_view_base,vil_image_view_base,vxl_int_32,vxl_int_32> op(abs_int);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    img_out->put_view(out);
  }

  // and finally the "floating point" types
  //
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
    std::cerr << __FILE__ ": vepl_monadic_abs() not implemented for " << image << '\n';

  return img_out;
}

vil_image_resource_sptr vepl_monadic_sqrt(const vil_image_resource_sptr& image)
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
    std::cerr << __FILE__ ": vepl_monadic_sqrt() not implemented for " << image << '\n';

  return img_out;
}

vil_image_resource_sptr vepl_monadic_sqr(const vil_image_resource_sptr& image)
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
    std::cerr << __FILE__ ": vepl_monadic_sqr() not implemented for " << image << '\n';

  return img_out;
}

vil_image_resource_sptr vepl_monadic_shear(const vil_image_resource_sptr& image, double shift, double scale)
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
    std::cerr << __FILE__ ": vepl_monadic_shear() not implemented for " << image << '\n';

  return img_out;
}
