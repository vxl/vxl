// This is tbl/vepl/vepl_dyadic.cxx
#include "vepl_dyadic.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_dyadic.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

typedef unsigned char ubyte;
typedef unsigned short ushort;
#define r_g_b vil1_rgb<ubyte> // cannot use typedef since that may cause ambiguous overload problems
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

void vepl_dyadic_sum(vil1_image im_out, vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ubyte,ubyte> op(sum_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
    vil1_memory_image_of<r_g_b > mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,r_g_b,r_g_b > op(sum_rgb);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    typedef unsigned short ushort;
    vil1_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ushort,ushort> op(sum_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,float,float> op(sum_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,double,double> op(sum_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_sum() not implemented for " << image << vcl_endl;
  }
}

void vepl_dyadic_dif(vil1_image im_out, vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ubyte,ubyte> op(dif_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
    vil1_memory_image_of<r_g_b > mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,r_g_b,r_g_b > op(dif_rgb);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    typedef unsigned short ushort;
    vil1_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ushort,ushort> op(dif_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,float,float> op(dif_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,double,double> op(dif_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_dif() not implemented for " << image << vcl_endl;
  }
}

void vepl_dyadic_min(vil1_image im_out, vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ubyte,ubyte> op(min_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    typedef unsigned short ushort;
    vil1_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ushort,ushort> op(min_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,float,float> op(min_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,double,double> op(min_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_min() not implemented for " << image << vcl_endl;
  }
}

void vepl_dyadic_max(vil1_image im_out, vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ubyte,ubyte> op(max_ubyte);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    typedef unsigned short ushort;
    vil1_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,ushort,ushort> op(max_ushort);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,float,float> op(max_float);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    im_out = mem;
    vipl_dyadic<vil1_image,vil1_image,double,double> op(max_double);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&im_out);
    op.filter();
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dyadic_max() not implemented for " << image << vcl_endl;
  }
}

