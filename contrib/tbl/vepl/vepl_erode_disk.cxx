#include "vepl_erode_disk.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_erode_disk.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

vil1_image vepl_erode_disk(vil1_image const& image, float radius)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    typedef unsigned char ubyte;
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<ubyte> out(image);
    vipl_erode_disk<vil1_image,vil1_image,ubyte,ubyte> op(radius);
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
    vipl_erode_disk<vil1_image,vil1_image,ubyte,ubyte> op(radius);
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
    vipl_erode_disk<vil1_image,vil1_image,ushort,ushort> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

#if 0 // currently no erosion for 32-bit integers
  // int
  else if (vil1_pixel_format(image) == VIL1_UINT32) {
    vil1_memory_image_of<unsigned> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<unsigned> out(image);
    vipl_erode_disk<vil1_image,vil1_image,unsigned,unsigned> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }
#endif

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_erode_disk<vil1_image,vil1_image,float,float> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_erode_disk<vil1_image,vil1_image,double,double> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_erode_disk() not implemented for " << image << vcl_endl;
    return 0;
  }
}

