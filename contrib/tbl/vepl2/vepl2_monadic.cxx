// This is tbl/vepl2/vepl2_monadic.cxx
#include "vepl2_monadic.h"
#include <vepl2/accessors/vipl_accessors_vil2_image_view_base.h>
#include <vipl/vipl_monadic.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_rgb.h>
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


vil2_image_view_base_sptr vepl2_monadic_abs(vil2_image_view_base const& image)
{
  // byte rgb
  if (image.pixel_format() == VIL2_PIXEL_FORMAT_RGB_BYTE) {
    typedef vil2_rgb<vxl_byte> r_g_b;
    vil2_image_view<r_g_b>* im = new vil2_image_view<r_g_b>;
    im->deep_copy((vil2_image_view<vxl_byte>const&)(image));
    // vxl_byte is unsigned so nothing happens
    return im;
  }

  // byte greyscale
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* im = new vil2_image_view<vxl_byte>;
    im->deep_copy((vil2_image_view<vxl_byte>const&)(image));
    // vxl_byte is unsigned so nothing happens
    return im;
  }

  // short
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    vil2_image_view<vxl_uint_16>* im = new vil2_image_view<vxl_uint_16>;
    im->deep_copy((vil2_image_view<vxl_uint_16>const&)(image));
    // vxl_uint_16 is unsigned so nothing happens
    return im;
  }

  // int
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32) {
    vil2_image_view<vxl_uint_32>* im = new vil2_image_view<vxl_uint_32>;
    im->deep_copy((vil2_image_view<vxl_uint_32>const&)(image));
    // vxl_uint_32 is unsigned so nothing happens
    return im;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,float,float> op(abs_float);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,double,double> op(abs_double);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_monadic_abs() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

vil2_image_view_base_sptr vepl2_monadic_sqrt(vil2_image_view_base const& image)
{
  // float
  if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,float,float> op(sqrt_float);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,double,double> op(sqrt_double);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_monadic_sqrt() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

vil2_image_view_base_sptr vepl2_monadic_sqr(vil2_image_view_base const& image)
{
  // byte greyscale
  if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte> op(sqr_ubyte);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    vil2_image_view<vxl_uint_16>* out = new vil2_image_view<vxl_uint_16>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,vxl_uint_16,vxl_uint_16> op(sqr_short);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // int
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32) {
    vil2_image_view<vxl_uint_32>* out = new vil2_image_view<vxl_uint_32>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,vxl_uint_32,vxl_uint_32> op(sqr_int);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,float,float> op(sqr_float);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,double,double> op(sqr_double);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_monadic_sqr() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

vil2_image_view_base_sptr vepl2_monadic_shear(vil2_image_view_base const& image, double shift, double scale)
{
  shift_ = shift; scale_ = scale;

  // byte greyscale
  if (image.pixel_format() == VIL2_PIXEL_FORMAT_BYTE) {
    vil2_image_view<vxl_byte>* out = new vil2_image_view<vxl_byte>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,vxl_byte,vxl_byte> op(shear_ubyte);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // short
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_16) {
    vil2_image_view<vxl_uint_16>* out = new vil2_image_view<vxl_uint_16>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,vxl_uint_16,vxl_uint_16> op(shear_short);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // int
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_UINT_32) {
    vil2_image_view<vxl_uint_32>* out = new vil2_image_view<vxl_uint_32>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,vxl_uint_32,vxl_uint_32> op(shear_int);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // float
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_FLOAT) {
    vil2_image_view<float>* out = new vil2_image_view<float>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,float,float> op(shear_float);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  // double
  else if (image.pixel_format() == VIL2_PIXEL_FORMAT_DOUBLE) {
    vil2_image_view<double>* out = new vil2_image_view<double>(image.ni(),image.nj(),image.nplanes());
    vipl_monadic<vil2_image_view_base,vil2_image_view_base,double,double> op(shear_double);
    op.put_in_data_ptr(&image);
    op.put_out_data_ptr(out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl2_monadic_shear() not implemented for " << image.is_a() << '\n';
    return 0;
  }
}

