//:
// \file
// \brief Example of loading an image and correlating with a normalised mask
// \author Tim Cootes

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/algo/vil2_normalised_correlation_2d.h>
#include <vil2/algo/vil2_sobel_3x3.h>
#include <vil2/vil2_convert.h>

int main(int argc, char** argv)
{
  if (argc!=4)
  {
    vcl_cout<<"vil2_correlate_grad_tool  src_image kernel_image dest_image\n";
    vcl_cout<<"Correlates gradient of kernel_image with gradient of src_image,"
            <<" saves to dest_image\n";
    return 0;
  }

  vil2_image_view<vxl_byte> src_im = vil2_load(argv[1]);
  if (src_im.size()==0)
  {
    vcl_cout<<"Unable to load source image from "<<argv[1]<<'\n';
    return 1;
  }

  // Create a normalised kernel gradient image
  vil2_image_view<float> src_im_f,s_grad_x,s_grad_y;
  vil2_convert_cast(src_im,src_im_f);
  vil2_sobel_3x3(src_im_f,s_grad_x,s_grad_y);

  vil2_image_view<vxl_byte> kernel_im_byte = vil2_load(argv[2]);
  if (kernel_im_byte.size()==0)
  {
    vcl_cout<<"Unable to load kernel image from "<<argv[1]<<'\n';
    return 1;
  }

  vcl_cout<<"Kernel image is "<<kernel_im_byte.ni()<<" x "<<kernel_im_byte.nj()<<'\n';

  // Create a normalised kernel gradient image
  vil2_image_view<float> kernel_image,k_grad_x,k_grad_y;
  vil2_convert_cast(kernel_im_byte,kernel_image);
  vil2_sobel_3x3(kernel_image,k_grad_x,k_grad_y);

  vil2_math_normalise(k_grad_x);
  vil2_math_normalise(k_grad_y);

  vil2_image_view<float> dest_image,dest_x,dest_y;

  vil2_normalised_correlation_2d(s_grad_x,dest_x,k_grad_x,double());
  vil2_normalised_correlation_2d(s_grad_y,dest_y,k_grad_y,double());

  vil2_math_image_sum(dest_x,dest_y,dest_image);

  // Stretch range to [0,255]
  float min_v,max_v;
  vil2_math_value_range(dest_image,min_v,max_v);
  double s = 255/(max_v-min_v);
  vil2_math_scale_and_offset_values(dest_image,s,-s*min_v);
  vcl_cout<<"Range of result: "<<min_v<<","<<max_v<<'\n';

  vil2_image_view<vxl_byte> result_image;
  vil2_convert_cast(dest_image,result_image);

  vcl_cout<<"Resulting image is "<<result_image.ni()<<" x "<<result_image.nj()<<'\n';

  if (!vil2_save(result_image, argv[3]))
  {
    vcl_cerr<<"Unable to save result image to "<<argv[3]<<'\n';
    return 1;
  }

  vcl_cout<<"Saved result of convolution to "<<argv[3]<<'\n';

  return 0;
}
