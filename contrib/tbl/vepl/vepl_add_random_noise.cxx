// This is tbl/vepl/vepl_add_random_noise.cxx
#include "vepl_add_random_noise.h"
//:
// \file

#include <vipl/accessors/vipl_accessors_vil1_image.h>
#include <vipl/vipl_add_random_noise.h>
#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_rgb.h>
#include <vxl_config.h> // for vxl_byte

vil1_image vepl_add_random_noise(vil1_image const& image, double maxdev)
{
  // byte greyscale
  if (vil1_pixel_format(image) == VIL1_BYTE)
  {
    vil1_memory_image_of<vxl_byte> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<vxl_byte> out(image);
    vipl_add_random_noise<vil1_image,vil1_image,vxl_byte,vxl_byte>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // byte rgb
  else if (vil1_pixel_format(image) == VIL1_RGB_BYTE)
  {
    vil1_memory_image_of<vil1_rgb<vxl_byte> > in(image); // load in memory to pass to filter
    vil1_memory_image_of<vil1_rgb<vxl_byte> > out(image);
    vil1_memory_image_of<vxl_byte> mem((vxl_byte*)(in.get_buffer()),3*in.width(),in.height()); // reinterpret as vxl_byte
    vil1_memory_image_of<vxl_byte> mout((vxl_byte*)(out.get_buffer()),3*in.width(),in.height());
    vipl_add_random_noise<vil1_image,vil1_image,vxl_byte,vxl_byte>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&mout);
    op.filter();
    return out;
  }

  // float
  else if (vil1_pixel_format(image) == VIL1_FLOAT)
  {
    vil1_memory_image_of<float> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<float> out(image);
    vipl_add_random_noise<vil1_image,vil1_image,float,float>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  // double
  else if (vil1_pixel_format(image) == VIL1_DOUBLE)
  {
    vil1_memory_image_of<double> mem(image); // load in memory to pass to filter
    vil1_memory_image_of<double> out(image);
    vipl_add_random_noise<vil1_image,vil1_image,double,double>
      op(GAUSSIAN_NOISE,maxdev);
    op.put_in_data_ptr(&mem);
    op.put_out_data_ptr(&out);
    op.filter();
    return out;
  }

  //
  else
  {
    vcl_cerr << __FILE__ ": vepl_add_random_noise() not implemented for " << image << vcl_endl;
    return 0;
  }
}

