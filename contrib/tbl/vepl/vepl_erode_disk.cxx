#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vepl/vepl_erode_disk.h>
#include <vipl/vipl_erode_disk.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

typedef unsigned char ubyte;
static inline vil_rgb<ubyte> vcl_min(vil_rgb<ubyte> const& a, vil_rgb<ubyte> const& b)
{ return vil_rgb<ubyte>(vcl_min(a.r,b.r), vcl_min(a.g,b.g), vcl_min(a.b,b.b)); }

vil_image vepl_erode_disk(vil_image const& image, float radius)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_erode_disk<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vil_memory_image_of<vil_rgb<ubyte> > mem(image); // load in memory to pass to filter
    vil_memory_image_of<vil_rgb<ubyte> > out(image);
    vipl_erode_disk<vil_image,vil_image,vil_rgb<ubyte> ,vil_rgb<ubyte> ,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_erode_disk<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_erode_disk<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(radius);
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

