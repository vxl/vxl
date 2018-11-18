// This is tbl/vepl/vepl_histogram.cxx
#include <iostream>
#include "vepl_histogram.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vepl/accessors/vipl_accessors_vil_image_view_base.h>
#include <vipl/vipl_histogram.h>
#include <vil/vil_pixel_format.h>
#include <vxl_config.h> // for vxl_byte

std::vector<unsigned int> vepl_histogram(const vil_image_resource_sptr& image)
{
  // byte rgb
  if (image->nplanes() > 1 || image->pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE) {
    std::cerr << __FILE__ ": vepl_histogram() cannot be implemented for colour images\n";
    return std::vector<unsigned int>();
  }

  // byte greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
    vil_image_view<vxl_byte> in = image->get_view();
    std::vector<unsigned int> out(256);
    vipl_histogram<vil_image_view_base,std::vector<unsigned int>, vxl_byte,unsigned int> op;
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // short greyscale
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
    vil_image_view<vxl_uint_16> in = image->get_view();
    std::vector<unsigned int> out(65536);
    vipl_histogram<vil_image_view_base,std::vector<unsigned int>, vxl_uint_16,unsigned int> op;
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float, pixel values between 0 and 1
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
    vil_image_view<float> in = image->get_view();
    std::vector<unsigned int> out(1000);
    // Must scale the pixel values from [0,1] to [0,1000]
    vipl_histogram<vil_image_view_base,std::vector<unsigned int>, float,unsigned int> op(0.001f);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double, pixel values between 0 and 1
  else if (image->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
    vil_image_view<double> in = image->get_view();
    std::vector<unsigned int> out(1000);
    // Must scale the pixel values from [0,1] to [0,1000]
    vipl_histogram<vil_image_view_base,std::vector<unsigned int>, double,unsigned int> op(0.001);
    op.put_in_data_ptr(&in);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    std::cerr << __FILE__ ": vepl_histogram() not implemented for " << image << '\n';
    return std::vector<unsigned int>();
  }
}
