//:
// \file
// \brief Example of loading an image and applying gradient filter
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <testlib/testlib_root_dir.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/algo/vil2_exp_grad_filter_1d.h>
#include <vil2/vil2_convert.h>
#include <vil2/vil2_print.h>

int main(int argc, char** argv)
{
  if (argc!=3)
  {
    vcl_cout<<"vil2_exp_grad_filter_tool  src_image x_grad_image\n";
    vcl_cout<<"Applies exponential gradient filter to src_image, saves to x_grad_image\n";
    return 0;
  }

  vil2_image_view<vxl_byte> src_im = vil2_load(argv[1]);
  if (src_im.size()==0)
  {
    vcl_cout<<"Unable to load source image from "<<argv[1]<<vcl_endl;
    return 1;
  }

  double k = 0.80;  // Kernel smoothing parameter

  vil2_image_view<float> grad_x;

  vil2_exp_grad_filter_i(src_im,grad_x,k,double());

  // Stretch range to [0,255]
  float min_v,max_v;
  vil2_math_value_range(grad_x,min_v,max_v);
  double s = 255/(max_v-min_v);
  vil2_math_scale_and_offset_values(grad_x,s,-s*min_v);
  vcl_cout<<"Range of result: "<<min_v<<","<<max_v<<vcl_endl;

  vil2_image_view<vxl_byte> grad_x_byte;
  vil2_convert_cast(grad_x,grad_x_byte);

  vcl_cout<<"Resulting image is "<<grad_x_byte.ni()<<" x "<<grad_x_byte.nj()<<vcl_endl;

  if (!vil2_save(grad_x_byte, argv[2]))
  {
    vcl_cerr<<"Unable to save result image to "<<argv[2]<<vcl_endl;
    return 1;
  }

  vcl_cout<<"Saved x-gradient to "<<argv[2]<<vcl_endl;

  return 0;
}
