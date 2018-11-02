//:
// \file
// \brief Example of loading an image and convolving with a mask
// \author Tim Cootes

#include <iostream>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_normalised_correlation_2d.h>
#include <vil/vil_convert.h>

int main(int argc, char** argv)
{
  if (argc!=4)
  {
    std::cout<<"vil_convolve_tool  src_image kernel_image dest_image\n"
            <<"Convolves kernel_image with src_image, saves to dest_image\n";
    return 0;
  }

  vil_image_view<vxl_byte> src_im = vil_load(argv[1]);
  if (src_im.size()==0)
  {
    std::cout<<"Unable to load source image from "<<argv[1]<<std::endl;
    return 1;
  }

  vil_image_view<vxl_byte> kernel_im_byte = vil_load(argv[2]);
  if (kernel_im_byte.size()==0)
  {
    std::cout<<"Unable to load kernel image from "<<argv[1]<<std::endl;
    return 1;
  }

  std::cout<<"Kernel image is "<<kernel_im_byte.ni()<<" x "<<kernel_im_byte.nj()<<std::endl;

  // Create a normalised kernel image
  vil_image_view<float> kernel_image;
  vil_convert_cast(kernel_im_byte,kernel_image);
  float mean,var;
  vil_math_mean_and_variance(mean,var,kernel_image,0);
  double ks = 1.0/std::sqrt(var);
  vil_math_scale_and_offset_values(kernel_image,ks,-ks*mean);

  vil_image_view<float> dest_image;

  vil_normalised_correlation_2d(src_im,dest_image,kernel_image,double());

  // Stretch range to [0,255]
  float min_v,max_v;
  vil_math_value_range(dest_image,min_v,max_v);
  double s = 255/(max_v-min_v);
  vil_math_scale_and_offset_values(dest_image,s,-s*min_v);
  std::cout<<"Range of result: "<<min_v<<','<<max_v<<std::endl;

  vil_image_view<vxl_byte> result_image;
  vil_convert_cast(dest_image,result_image);

  std::cout<<"Resulting image is "<<result_image.ni()<<" x "<<result_image.nj()<<std::endl;

  if (!vil_save(result_image, argv[3]))
  {
    std::cerr<<"Unable to save result image to "<<argv[3]<<std::endl;
    return 1;
  }

  std::cout<<"Saved result of convolution to "<<argv[3]<<std::endl;

  return 0;
}
