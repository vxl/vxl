//:
// \file
// \brief Example of loading an image and correlating with a normalised mask
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vul/vul_arg.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_normalised_correlation_2d.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vil/vil_convert.h>
#include <vil/algo/vil_suppress_non_max.h>

void print_usage()
{
  vcl_cout<<"vil_correlate_grad_tool  -i src_image -k kernel_image -o dest_image [-s]\n"
          <<"Correlates gradient of kernel_image with gradient of\n"
          <<"src_image, saves to dest_image\n";
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> in_path("-i","Input image");
  vul_arg<vcl_string> kernel_path("-k","Kernel image");
  vul_arg<vcl_string> out_path("-o","Output image","output.jpg");
  vul_arg<bool> suppress_non_max("-s","Suppress non-peaks",false);
  vul_arg_parse(argc, argv);

  if (in_path() == "" || kernel_path()=="")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> src_im = vil_load(in_path().c_str());
  if (src_im.size()==0)
  {
    vcl_cout<<"Unable to load source image from "<<in_path()<<'\n';
    return 1;
  }

  // Create a normalised kernel gradient image
  vil_image_view<float> src_im_f,s_grad_x,s_grad_y;
  vil_convert_cast(src_im,src_im_f);
  vil_sobel_3x3(src_im_f,s_grad_x,s_grad_y);

  vil_image_view<vxl_byte> kernel_im_byte = vil_load(kernel_path().c_str());
  if (kernel_im_byte.size()==0)
  {
    vcl_cout<<"Unable to load kernel image from "<<kernel_path()<<'\n';
    return 1;
  }

  vcl_cout<<"Kernel image is "<<kernel_im_byte.ni()<<" x "<<kernel_im_byte.nj()<<'\n';

  // Create a normalised kernel gradient image
  vil_image_view<float> kernel_image,k_grad_x,k_grad_y;
  vil_convert_cast(kernel_im_byte,kernel_image);
  vil_sobel_3x3(kernel_image,k_grad_x,k_grad_y);

  vil_math_normalise(k_grad_x);
  vil_math_normalise(k_grad_y);

  vil_image_view<float> dest_image,dest_x,dest_y;

  vil_normalised_correlation_2d(s_grad_x,dest_x,k_grad_x,double());
  vil_normalised_correlation_2d(s_grad_y,dest_y,k_grad_y,double());

  vil_math_image_sum(dest_x,dest_y,dest_image);

  if (suppress_non_max())
  {
    vil_image_view<float> snm_image;
    vil_suppress_non_max_3x3(dest_image,snm_image);
    dest_image=snm_image;
  }

  // Stretch range to [0,255]
  float min_v,max_v;
  vil_math_value_range(dest_image,min_v,max_v);
  double s = 255/(max_v-min_v);
  vil_math_scale_and_offset_values(dest_image,s,-s*min_v);
  vcl_cout<<"Range of result: "<<min_v<<","<<max_v<<'\n';

  vil_image_view<vxl_byte> result_image;
  vil_convert_cast(dest_image,result_image);

  vcl_cout<<"Resulting image is "<<result_image.ni()<<" x "<<result_image.nj()<<'\n';

  if (!vil_save(result_image, out_path().c_str()))
  {
    vcl_cerr<<"Unable to save result image to "<<out_path()<<'\n';
    return 1;
  }

  vcl_cout<<"Saved result of convolution to "<<out_path()<<'\n';

  return 0;
}
