// This is tbl/vepl/vepl_monadic.cxx
#include "vepl_monadic.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_monadic.h>
#include <vil1/vil1_memory_image_of.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte

float abs_float(float const& a) { return (a<0) ? -a : a; }
double abs_double(double const& a) { return vcl_fabs(a); }
float sqrt_float(float const& a) { return (a<0) ? -vcl_sqrt(a) : vcl_sqrt(a); }
double sqrt_double(double const& a) { return (a<0) ? -vcl_sqrt(a) : vcl_sqrt(a); }
vxl_byte sqr_ubyte(vxl_byte const& a) { return a*a; }
vxl_uint_16 sqr_ushort(vxl_uint_16 const& a) { return a*a; }
float sqr_float(float const& a) { return a*a; }
double sqr_double(double const& a) { return a*a; }
static double shift_=0.0, scale_=1.0;
vxl_byte shear_ubyte(vxl_byte const& a) { return vxl_byte((a+shift_)*scale_); }
vxl_uint_16 shear_ushort(vxl_uint_16 const& a) { return vxl_uint_16((a+shift_)*scale_); }
float shear_float(float const& a) { return float((a+shift_)*scale_); }
double shear_double(double const& a) { return (a+shift_)*scale_; }


vil1_image vepl_monadic_abs(vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    return image; // vxl_byte is unsigned so nothing happens
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
    return image; // vxl_byte is unsigned so nothing happens
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    return image; // vxl_uint_16 is unsigned so nothing happens
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_monadic<vil1_image,vil1_image,float,float> op(abs_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_monadic<vil1_image,vil1_image,double,double> op(abs_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_monadic_abs() not implemented for " << image << vcl_endl;
    return 0;
  }
}

vil1_image vepl_monadic_sqrt(vil1_image const& image)
{
  // float
  if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_monadic<vil1_image,vil1_image,float,float> op(sqrt_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_monadic<vil1_image,vil1_image,double,double> op(sqrt_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_monadic_sqrt() not implemented for " << image << vcl_endl;
    return 0;
  }
}

vil1_image vepl_monadic_sqr(vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<vxl_byte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_byte> out(image);
    vipl_monadic<vil1_image,vil1_image,vxl_byte,vxl_byte> op(sqr_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    vil1_memory_image_of<vxl_uint_16> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_uint_16> out(image);
    vipl_monadic<vil1_image,vil1_image,vxl_uint_16,vxl_uint_16> op(sqr_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_monadic<vil1_image,vil1_image,float,float> op(sqr_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_monadic<vil1_image,vil1_image,double,double> op(sqr_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_monadic_sqr() not implemented for " << image << vcl_endl;
    return 0;
  }
}

vil1_image vepl_monadic_shear(vil1_image const& image, double shift, double scale)
{
  shift_ = shift; scale_ = scale;

  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<vxl_byte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_byte> out(image);
    vipl_monadic<vil1_image,vil1_image,vxl_byte,vxl_byte> op(shear_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    vil1_memory_image_of<vxl_uint_16> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_uint_16> out(image);
    vipl_monadic<vil1_image,vil1_image,vxl_uint_16,vxl_uint_16> op(shear_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_monadic<vil1_image,vil1_image,float,float> op(shear_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_monadic<vil1_image,vil1_image,double,double> op(shear_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_monadic_shear() not implemented for " << image << vcl_endl;
    return 0;
  }
}

