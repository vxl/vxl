#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vepl/vepl_monadic.h>
#include <vipl/vipl_monadic.h>
#include <vil/vil_memory_image_of.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

typedef unsigned char ubyte;
float abs_float(float const& a) { return (a<0) ? -a : a; }
double abs_double(double const& a) { return vcl_fabs(a); }
float sqrt_float(float const& a) { return (a<0) ? -vcl_sqrt(a) : vcl_sqrt(a); }
double sqrt_double(double const& a) { return (a<0) ? -vcl_sqrt(a) : vcl_sqrt(a); }
ubyte sqr_ubyte(ubyte const& a) { return a*a; }
float sqr_float(float const& a) { return a*a; }
double sqr_double(double const& a) { return a*a; }
static double shift_=0.0, scale_=1.0;
ubyte shear_ubyte(ubyte const& a) { return ubyte((a+shift_)*scale_); }
float shear_float(float const& a) { return float((a+shift_)*scale_); }
double shear_double(double const& a) { return (a+shift_)*scale_; }


vil_image vepl_monadic_abs(vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    return image; // ubyte is unsigned so nothing happens
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    return image; // ubyte is unsigned so nothing happens
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_monadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(abs_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_monadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(abs_double);
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

vil_image vepl_monadic_sqrt(vil_image const& image)
{
  // float
  if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_monadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(sqrt_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_monadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(sqrt_double);
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

vil_image vepl_monadic_sqr(vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_monadic<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(sqr_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_monadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(sqr_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_monadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(sqr_double);
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

vil_image vepl_monadic_shear(vil_image const& image, double shift, double scale)
{
  shift_ = shift; scale_ = scale;

  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_monadic<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(shear_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_monadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(shear_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_monadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(shear_double);
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

