#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vepl/vepl_dilate_disk.h>
#include <vipl/vipl_dilate_disk.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

typedef unsigned char ubyte;
#if VCL_ALLOWS_NAMESPACE_STD
namespace std { static inline vil_rgb<ubyte> max
#else
static inline vil_rgb<ubyte> vcl_max
#endif
(vil_rgb<ubyte> const& a, vil_rgb<ubyte> const& b)
  //recursive: { return vil_rgb<ubyte>(vcl_max(a.r,b.r), vcl_max(a.g,b.g), vcl_max(a.b,b.b)); }
{
  return vil_rgb<ubyte>(a.r > b.r ? a.r : b.r,
                        a.g > b.g ? a.g : b.g,
                        a.b > b.b ? a.b : b.b);
}
#if VCL_ALLOWS_NAMESPACE_STD
}
#else
#endif

vil_image vepl_dilate_disk(vil_image const& image, float radius)
{
  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_dilate_disk<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    vil_memory_image_of<vil_rgb<ubyte> > mem(image); // load in memory to pass to filter
    vil_memory_image_of<vil_rgb<ubyte> > out(image);
    vipl_dilate_disk<vil_image,vil_image,vil_rgb<ubyte> ,vil_rgb<ubyte> ,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_dilate_disk<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_dilate_disk<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(radius);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_dilate_disk() not implemented for " << image << vcl_endl;
    return 0;
  }
}

