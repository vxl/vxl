// This is tbl/vepl/vepl_add_random_noise.cxx
#include "vepl_add_random_noise.h"
//:
// \file

#include <vipl/accessors/vipl_accessors_vil_image.h>
#include <vipl/vipl_add_random_noise.h>
#include <vil/vil_memory_image_of.h>
#include <vil/vil_rgb.h>

vil_image vepl_add_random_noise(vil_image const& image, double maxdev)
{
  static int seed = 12345;

  // byte greyscale
  if (vil_pixel_format(image) == VIL_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<ubyte> mem(image); // load in memory to pass to filter
    vil_memory_image_of<ubyte> out(image);
    vipl_add_random_noise<vil_image,vil_image,ubyte,ubyte,vipl_trivial_pixeliter>
      op(GAUSSIAN_NOISE,(ubyte)(0.5+maxdev),seed);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil_pixel_format(image) == VIL_RGB_BYTE) {
    typedef unsigned char ubyte;
    vil_memory_image_of<vil_rgb<ubyte> > mem(image); // load in memory to pass to filter
    vil_memory_image_of<vil_rgb<ubyte> > out(image);
    vil_rgb<ubyte> m ((ubyte)(0.5+maxdev),(ubyte)(0.5+maxdev),(ubyte)(0.5+maxdev));
    vipl_add_random_noise<vil_image,vil_image,vil_rgb<ubyte>,vil_rgb<ubyte>,vipl_trivial_pixeliter>
      op(GAUSSIAN_NOISE,m,seed);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // float
  else if (vil_pixel_format(image) == VIL_FLOAT) {
    vil_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil_memory_image_of<float> out(image);
    vipl_add_random_noise<vil_image,vil_image,float,float,vipl_trivial_pixeliter> op(GAUSSIAN_NOISE,(float)maxdev,seed);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil_pixel_format(image) == VIL_DOUBLE) {
    vil_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil_memory_image_of<double> out(image);
    vipl_add_random_noise<vil_image,vil_image,double,double,vipl_trivial_pixeliter> op(GAUSSIAN_NOISE,maxdev,seed);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else {
    vcl_cerr << __FILE__ ": vepl_add_random_noise() not implemented for " << image << vcl_endl;
    return 0;
  }
}

