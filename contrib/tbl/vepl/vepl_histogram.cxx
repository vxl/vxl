// This is tbl/vepl/vepl_histogram.cxx
#include "vepl_histogram.h"
#include <vcl_iostream.h>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_histogram.h>
#include <vil1/vil1_memory_image_of.h>
#include <vxl_config.h> // for vxl_byte

vcl_vector<unsigned int> vepl_histogram(vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    vil1_memory_image_of<vxl_byte> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out(256);
    vipl_histogram<vil1_image,vcl_vector<unsigned int>, vxl_byte,unsigned int> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // short greyscale
  if (vil1_pixel_format(image) == VIL1_UINT16) {
    vil1_memory_image_of<vxl_uint_16> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out(65536);
    vipl_histogram<vil1_image,vcl_vector<unsigned int>, vxl_uint_16,unsigned int> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
    vcl_cerr << __FILE__ ": vepl_histogram() cannot be implemented for colour images\n";
    return vcl_vector<unsigned int>();
  }

  // float, pixel values between 0 and 1
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out(1000);
    // Must scale the pixel values from [0,1] to [0,1000]
    vipl_histogram<vil1_image,vcl_vector<unsigned int>, float,unsigned int> op(0.001f);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double, pixel values between 0 and 1
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out(1000);
    // Must scale the pixel values from [0,1] to [0,1000]
    vipl_histogram<vil1_image,vcl_vector<unsigned int>, double,unsigned int> op(0.001);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_histogram() not implemented for " << image << vcl_endl;
    return vcl_vector<unsigned int>();
  }
}

