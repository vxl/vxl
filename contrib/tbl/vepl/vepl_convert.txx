// This is tbl/vepl/vepl_convert.txx
#ifndef vepl_convert_txx_
#define vepl_convert_txx_

#include "vepl_convert.h"
#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_convert.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

template <class D>
vil_image vepl_convert(vil_image const& image, D dummy)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<D > out(image);
    vipl_convert<vil_image,vil_image,ubyte,D> op;
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
    vil_memory_image_of<D > out(image);
    vipl_convert<vil_image,vil_image,r_g_b,D> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // 16-bit greyscale
  else if (vil_pixel_format(image) == VIL_UINT16) {
    typedef unsigned short ushort;
    vil_memory_image_of<ushort> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ushort> out(image);
    vipl_convert<vil_image,vil_image,ushort,D> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<D > out(image);
    vipl_convert<vil_image,vil_image,float,D> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<D > out(image);
    vipl_convert<vil_image,vil_image,double,D> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_convert() not implemented for " << image << vcl_endl;
    return 0;
  }
}

#endif // vepl_convert_txx_
