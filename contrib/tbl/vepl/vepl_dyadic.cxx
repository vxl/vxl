// This is tbl/vepl/vepl_dyadic.cxx
#include "vepl_dyadic.h"
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_dyadic.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef vil_rgb<ubyte> r_g_b;
void sum_ubyte(ubyte& a, ubyte const& b) { a += b; }
void sum_ushort(ushort& a, ushort const& b) { a += b; }
void sum_rgb(r_g_b& a, r_g_b const& b) { a.r += b.r; a.g += b.g; a.b += b.b; }
void sum_float(float& a, float const& b) { a += b; }
void sum_double(double& a, double const& b) { a += b; }
void dif_ubyte(ubyte& a, ubyte const& b) { a -= b; }
void dif_ushort(ushort& a, ushort const& b) { a -= b; }
void dif_rgb(r_g_b& a, r_g_b const& b) { a.r -= b.r; a.g -= b.g; a.b -= b.b; }
void dif_float(float& a, float const& b) { a -= b; }
void dif_double(double& a, double const& b) { a -= b; }
void min_ubyte(ubyte& a, ubyte const& b) { if (b<a) a = b; }
void min_ushort(ushort& a, ushort const& b) { if (b<a) a = b; }
void min_float(float& a, float const& b) { if (b<a) a = b; }
void min_double(double& a, double const& b) { if (b<a) a = b; }
void max_ubyte(ubyte& a, ubyte const& b) { if (a<b) a = b; }
void max_ushort(ushort& a, ushort const& b) { if (a<b) a = b; }
void max_float(float& a, float const& b) { if (a<b) a = b; }
void max_double(double& a, double const& b) { if (a<b) a = b; }

void vepl_dyadic_sum(vil_image im_out, vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte> op(sum_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vil_memory_image_of<r_g_b> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,r_g_b,r_g_b> op(sum_rgb);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil_pixel_format(image) == VIL_UINT16) {
    typedef unsigned short ushort;
    vil_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ushort,ushort> op(sum_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,float,float> op(sum_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,double,double> op(sum_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
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
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte> op(dif_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vil_memory_image_of<r_g_b> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,r_g_b,r_g_b> op(dif_rgb);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil_pixel_format(image) == VIL_UINT16) {
    typedef unsigned short ushort;
    vil_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ushort,ushort> op(dif_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,float,float> op(dif_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,double,double> op(dif_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
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
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte> op(min_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil_pixel_format(image) == VIL_UINT16) {
    typedef unsigned short ushort;
    vil_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ushort,ushort> op(min_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,float,float> op(min_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,double,double> op(min_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
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
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ubyte,ubyte> op(max_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil_pixel_format(image) == VIL_UINT16) {
    typedef unsigned short ushort;
    vil_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,ushort,ushort> op(max_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,float,float> op(max_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil_image,vil_image,double,double> op(max_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_max() not implemented for " << image << vcl_endl;
  }
}

