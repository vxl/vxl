// This is tbl/vepl/vepl_threshold.cxx
#include "vepl_threshold.h"
#include <vcl_iostream.h>
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_threshold.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

vil_image vepl_threshold(vil_image const& image, double threshold, double below, double above)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_threshold<vil_image,vil_image,ubyte,ubyte> op((ubyte)(0.5+threshold), (ubyte)(0.5+below), (ubyte)(0.5+above));
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
    vipl_threshold<vil_image,vil_image,ubyte,ubyte> op((ubyte)(0.5+threshold), (ubyte)(0.5+below), (ubyte)(0.5+above));
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
    vipl_threshold<vil_image,vil_image,ushort,ushort> op((ushort)(0.5+threshold), (ushort)(0.5+below), (ushort)(0.5+above));
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_threshold<vil_image,vil_image,float,float> op((float)threshold, (float)below, (float)above);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_threshold<vil_image,vil_image,double,double> op(threshold, below, above);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_threshold() not implemented for " << image << vcl_endl;
    return 0;
  }
}

