#include "vepl_erode_disk.h"
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_erode_disk.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

vil_image vepl_erode_disk(vil_image const& image, float radius)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_erode_disk<vil_image,vil_image,ubyte,ubyte> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb: process colour bands independently as ubyte images
  else if (vil_pixel_format(image) == VIL_RGB_BYTE)
  {
    typedef unsigned char ubyte;
    typedef vil_rgb<ubyte> r_g_b;
    vil_memory_image_of<r_g_b> in(image); // load in memory to pass to filter
    vil_memory_image_of<r_g_b> out(image);
    vil_memory_image_of<ubyte> mem((ubyte*)(in.get_buffer()),3*in.width(),in.height()); // reinterpret as ubyte
    vil_memory_image_of<ubyte> mout((ubyte*)(out.get_buffer()),3*in.width(),in.height());
    vipl_erode_disk<vil_image,vil_image,ubyte,ubyte> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&mout);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil_pixel_format(image) == VIL_UINT16) {
    typedef unsigned short ushort;
    vil_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ushort> out(image);
    vipl_erode_disk<vil_image,vil_image,ushort,ushort> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

#if 0 // currently no erosion for 32-bit integers
  // int
  else if (vil_pixel_format(image) == VIL_UINT32) {
    vil_memory_image_of<unsigned> mem(image); // load in memory to pass to filter
    vil_memory_image_of<unsigned> out(image);
    vipl_erode_disk<vil_image,vil_image,unsigned,unsigned> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }
#endif

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_erode_disk<vil_image,vil_image,float,float> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_erode_disk<vil_image,vil_image,double,double> op(radius);
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

