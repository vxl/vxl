// This is tbl/vepl/vepl_dyadic.cxx
#include "vepl_dyadic.h"
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_dyadic.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

typedef unsigned char ubyte;
void sum_ubyte(ubyte& a, ubyte const& b) { a += b; }
void sum_rgb(vil_rgb<ubyte>& a, vil_rgb<ubyte> const& b) { a.r += b.r; a.g += b.g; a.b += b.b; }
void sum_float(float& a, float const& b) { a += b; }
void sum_double(double& a, double const& b) { a += b; }
void dif_ubyte(ubyte& a, ubyte const& b) { a -= b; }
void dif_rgb(vil_rgb<ubyte>& a, vil_rgb<ubyte> const& b) { a.r -= b.r; a.g -= b.g; a.b -= b.b; }
void dif_float(float& a, float const& b) { a -= b; }
void dif_double(double& a, double const& b) { a -= b; }
void min_ubyte(ubyte& a, ubyte const& b) { if (b<a) a = b; }
void min_float(float& a, float const& b) { if (b<a) a = b; }
void min_double(double& a, double const& b) { if (b<a) a = b; }
void max_ubyte(ubyte& a, ubyte const& b) { if (a<b) a = b; }
void max_float(float& a, float const& b) { if (a<b) a = b; }
void max_double(double& a, double const& b) { if (a<b) a = b; }

void vepl_dyadic_sum(vil_image im_out, vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(sum_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vil_memory_image_of<vil_rgb<ubyte> > mem(image); // load in memory to pass to filter
    vil_memory_image_of<vil_rgb<ubyte> > out(image);
    vipl_dyadic<vil_image,vil_image,vil_rgb<ubyte> ,vil_rgb<ubyte> ,vipl_trivial_pixeliter> op(sum_rgb);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_dyadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(sum_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_dyadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(sum_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_sum() not implemented for " << image << vcl_endl;
  }
}

void vepl_dyadic_dif(vil_image im_out, vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(dif_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vil_memory_image_of<vil_rgb<ubyte> > mem(image); // load in memory to pass to filter
    vil_memory_image_of<vil_rgb<ubyte> > out(image);
    vipl_dyadic<vil_image,vil_image,vil_rgb<ubyte> ,vil_rgb<ubyte> ,vipl_trivial_pixeliter> op(dif_rgb);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_dyadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(dif_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_dyadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(dif_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_dif() not implemented for " << image << vcl_endl;
  }
}

void vepl_dyadic_min(vil_image im_out, vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(min_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_dyadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(min_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_dyadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(min_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_min() not implemented for " << image << vcl_endl;
  }
}

void vepl_dyadic_max(vil_image im_out, vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(max_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_dyadic<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(max_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_dyadic<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(max_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_max() not implemented for " << image << vcl_endl;
  }
}

