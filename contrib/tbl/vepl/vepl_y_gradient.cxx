// This is tbl/vepl/vepl_y_gradient.cxx
#include "vepl_y_gradient.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_y_gradient.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vxl_config.h> // for vxl_byte

vil1_image vepl_y_gradient(vil1_image const& image, double scale, double shift)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<vxl_byte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_byte> out(image);
    vipl_y_gradient<vil1_image,vil1_image,vxl_byte,vxl_byte> op(scale, vxl_byte(shift+0.5));
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
#define r_g_b vil1_rgb<vxl_byte> // cannot use typedef since that may cause ambiguous overload problems
    vil1_memory_image_of<r_g_b > mem(image); // load in memory to pass to filter
    vil1_memory_image_of<r_g_b > out(image);
    vipl_y_gradient<vil1_image,vil1_image,r_g_b,r_g_b > op(scale, r_g_b(vxl_byte(shift+0.5)) );
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    vil1_memory_image_of<vxl_uint_16> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_uint_16> out(image);
    vipl_y_gradient<vil1_image,vil1_image,vxl_uint_16,vxl_uint_16> op(scale, vxl_uint_16(shift+0.5));
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_y_gradient<vil1_image,vil1_image,float,float> op(scale, (float)shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_y_gradient<vil1_image,vil1_image,double,double> op(scale, shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_y_gradient() not implemented for " << image << vcl_endl;
    return 0;
  }
}

