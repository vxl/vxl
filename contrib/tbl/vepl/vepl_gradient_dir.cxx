// This is tbl/vepl/vepl_gradient_dir.cxx
#include "vepl_gradient_dir.h"
#include <vcl_iostream.h>
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_gradient_dir.h>
#include <vil1/vil1_memory_image_of.h>
#include <vxl_config.h> // for vxl_byte

vil1_image vepl_gradient_dir(vil1_image const& image, double scale, double shift)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<vxl_byte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_byte> out(image);
    vipl_gradient_dir<vil1_image,vil1_image,vxl_byte,vxl_byte> op(scale, shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
    vcl_cerr << __FILE__ ": vepl_gradient_dir() cannot be implemented for colour images\n";
    return 0;
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    vil1_memory_image_of<vxl_uint_16> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_uint_16> out(image);
    vipl_gradient_dir<vil1_image,vil1_image,vxl_uint_16,vxl_uint_16> op(scale, shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_gradient_dir<vil1_image,vil1_image,float,float> op(scale, shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_gradient_dir<vil1_image,vil1_image,double,double> op(scale, shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_gradient_dir() not implemented for " << image << vcl_endl;
    return 0;
  }
}

