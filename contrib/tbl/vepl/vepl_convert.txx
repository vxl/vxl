// This is tbl/vepl/vepl_convert.txx
#ifndef vepl_convert_txx_
#define vepl_convert_txx_

#include "vepl_convert.h"
#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_convert.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>

template <class D>
vil1_image vepl_convert(vil1_image const& image, D /*dummy*/)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE) {
    typedef unsigned char ubyte;
    vil1_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<D > out(image);
    vipl_convert<vil1_image,vil1_image,ubyte,D> op;
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
    vil1_memory_image_of<D > out(image);
    vipl_convert<vil1_image,vil1_image,r_g_b,D> op;
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
    vipl_convert<vil1_image,vil1_image,ushort,D> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT) {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<D > out(image);
    vipl_convert<vil1_image,vil1_image,float,D> op;
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE) {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<D > out(image);
    vipl_convert<vil1_image,vil1_image,double,D> op;
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
