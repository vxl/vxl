// This is tbl/vepl/vepl_sobel.cxx
#include "vepl_sobel.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_sobel.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

vil1_image vepl_sobel(vil1_image const& image)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    typedef unsigned char ubyte;
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<ubyte> out(image);
    vipl_sobel<vil1_image,vil1_image,ubyte,ubyte> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb: process colour bands independently as ubyte images
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE)
  {
    typedef unsigned char ubyte;
#define r_g_b vil1_rgb<ubyte> // cannot use typedef since that may cause ambiguous overload problems
    vil1_memory_image_of<r_g_b > in(image); // load in memory to pass to filter
    vil1_memory_image_of<r_g_b > out(image);
    vil1_memory_image_of<ubyte> mem((ubyte*)(in.get_buffer()),3*in.width(),in.height()); // reinterpret as ubyte
    vil1_memory_image_of<ubyte> mout((ubyte*)(out.get_buffer()),3*in.width(),in.height());
    vipl_sobel<vil1_image,vil1_image,ubyte,ubyte> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&mout);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil1_pixel_format(image) == VIL1_UINT16) {
    typedef unsigned short ushort;
    vil1_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<ushort> out(image);
    vipl_sobel<vil1_image,vil1_image,ushort,ushort> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_sobel<vil1_image,vil1_image,float,float> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_sobel<vil1_image,vil1_image,double,double> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_sobel() not implemented for " << image << vcl_endl;
    return 0;
  }
}

