// This is tbl/vepl/vepl_y_gradient.cxx
#include "vepl_y_gradient.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_y_gradient.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

vil1_image vepl_y_gradient(vil1_image const& image, double scale, double shift)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    typedef unsigned char ubyte;
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<ubyte> out(image);
    vipl_y_gradient<vil1_image,vil1_image,ubyte,ubyte> op(scale, (ubyte)(shift+0.5));
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE) {
    typedef unsigned char ubyte;
#define r_g_b vil1_rgb<ubyte> // cannot use typedef since that may cause ambiguous overload problems
    vil1_memory_image_of<r_g_b > mem(image); // load in memory to pass to filter
    vil1_memory_image_of<r_g_b > out(image);
    vipl_y_gradient<vil1_image,vil1_image,r_g_b,r_g_b > op(scale, r_g_b((ubyte)(shift+0.5)) );
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    typedef unsigned short ushort;
    vil1_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<ushort> out(image);
    vipl_y_gradient<vil1_image,vil1_image,ushort,ushort> op(scale, (ushort)(shift+0.5));
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

