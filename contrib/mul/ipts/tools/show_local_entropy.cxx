//:
// \file
// \brief Experimental tool to load in an image and compute local entropy.
// \author Tim Cootes

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include <vgl/vgl_point_2d.h>
#include <ipts/ipts_local_entropy.h>
#include <vimt/algo/vimt_find_peaks.h>
#include <ipts/ipts_draw.h>

void print_usage()
{
  std::cout<<"show_local_entropy -i in_image -e entropy_image -o out_image -h half_width\n"
          <<"Load in an image and generate entropy from square regions.\n"
          <<"Show peaks in entropy as crosses on original image.\n";
}

int main( int argc, char* argv[] )
{
  vul_arg<std::string> in_path("-i","Input image");
  vul_arg<std::string> entropy_path("-e","Entropy image","entropy.jpg");
  vul_arg<std::string> out_path("-o","Output image","output.jpg");
  vul_arg<int> half_width("-h","Half width of RIO",10);
  vul_arg_parse(argc, argv);

  if (in_path() == "")
  {
    print_usage();
    vul_arg_display_usage_and_exit();
  }

  vil_image_view<vxl_byte> src_im = vil_load(in_path().c_str());
  if (src_im.size()==0)
  {
    std::cout<<"Unable to load source image from "<<in_path()<<std::endl;
    return 1;
  }

  vil_image_view<vxl_byte> grey_im;
  vil_math_mean_over_planes(src_im,grey_im);

  vil_image_view<float> entropy_im,entropy_max_im;
  ipts_local_entropy(grey_im,entropy_im,half_width());

//  vil_suppress_non_max_3x3(entropy_im,entropy_max_im);

  vil_image_view<vxl_byte> dest_im;
  vil_convert_stretch_range(entropy_im,dest_im);

  if (!vil_save(dest_im, entropy_path().c_str()))
  {
    std::cerr<<"Unable to save entropy image to "<<entropy_path()<<std::endl;
    return 1;
  }

  std::cout<<"Saved image to "<<entropy_path()<<std::endl;

  std::vector<vgl_point_2d<unsigned> > peaks;
  vimt_find_image_peaks_3x3(peaks,entropy_im);
  for (auto & peak : peaks)
    ipts_draw_cross(grey_im,peak.x()+half_width(),peak.y()+half_width(),half_width(),vxl_byte(255));

  if (!vil_save(grey_im, out_path().c_str()))
  {
    std::cerr<<"Unable to save result image to "<<out_path()<<std::endl;
    return 1;
  }

  std::cout<<"Saved image to "<<out_path()<<std::endl;

  return 0;
}
