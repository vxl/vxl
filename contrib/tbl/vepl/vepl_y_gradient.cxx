#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vepl/vepl_y_gradient.h>
#include <vipl/vipl_y_gradient.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

vil_image vepl_y_gradient(vil_image const& image, double scale, double shift)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_y_gradient<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(scale, (ubyte)(shift+0.5));
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    typedef unsigned char ubyte;
    typedef vil_rgb<ubyte> r_g_b;
    vil_memory_image_of<r_g_b> mem(image); // load in memory to pass to filter
    vil_memory_image_of<r_g_b> out(image);
    vipl_y_gradient<vil_image,vil_image,r_g_b,r_g_b,vipl_trivial_pixeliter> op(scale, r_g_b((ubyte)(shift+0.5)) );
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_y_gradient<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(scale, (float)shift);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_y_gradient<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(scale, shift);
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

