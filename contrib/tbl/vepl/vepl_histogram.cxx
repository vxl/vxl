#include <vcl_iostream.h>
#include <vipl/accessors/vipl_accessors_vcl_vector.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vepl/vepl_histogram.h>
#include <vipl/vipl_histogram.h>
#include <vil/vil_memory_image_of.h>

vcl_vector<unsigned int> vepl_histogram(vil_image const& image)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out;
    vipl_histogram<vil_image,vcl_vector<unsigned int>, ubyte,unsigned int> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vcl_cerr << __FILE__ ": vepl_histogram() cannot be implemented for colour images\n";
    return vcl_vector<unsigned int>();
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out;
    vipl_histogram<vil_image,vcl_vector<unsigned int>, float,unsigned int> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vcl_vector<unsigned int> out;
    vipl_histogram<vil_image,vcl_vector<unsigned int>, double,unsigned int> op;
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

