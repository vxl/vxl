//:
// \file
// \brief Load in an image and create four points at (or near) its corners.
// \author Luke Chaplin, Tim Cootes

#include <iostream>
#include <cstdlib>

#include <msm/msm_points.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_arg.h>
#include <vimt/vimt_load.h>


void print_usage()
{
  std::cout<<"msm_get_image_corners -i image.png -bi 1 -bj 1 -p corners.pts"<<std::endl;
  std::cout<<"Load in an image."<<std::endl;
  std::cout<<"Compute points at corners of image region, after shrinking by bi pixels in i and bj pixels in j"<<std::endl;
  std::cout<<"Points saved in world coordinates - (using the image-to-world transform where available)."<<std::endl;

  vul_arg_display_usage_and_exit();
}


int main(int argc, char** argv)
{
  vul_arg<std::string> image_path("-i","Image path");
  vul_arg<std::string> points_path("-p","Path to which to the corner points");
  vul_arg<int> bi("-bi","Border (along i)",1);
  vul_arg<int> bj("-bj","Border (along j)",1);
  vul_arg<float> us("-us","Unit scaling (e.g. to convert from m to mm",1);

  vul_arg_parse(argc,argv);

  if (image_path()=="" || points_path()=="")
  {
    print_usage();
    return 0;
  }

  // Load in image
  vimt_image_2d_of<vxl_byte> image;
  vimt_load_to_byte(image_path(), image, us());

  int ni=int(image.image().ni());
  int nj=int(image.image().nj());

  // Transformation from image to world
  vimt_transform_2d i2w=image.world2im().inverse();

  msm_points corners(4);
  corners.set_point(0,i2w(bi(),bj()));
  corners.set_point(1,i2w(ni-1-bi(),bj()));
  corners.set_point(2,i2w(bi(),nj-1-bj()));
  corners.set_point(3,i2w(ni-1-bi(),nj-1-bj()));

  if (!corners.write_text_file(points_path()))
  {
    std::cerr<<"Failed to write points to "<<points_path()<<std::endl;
    return 3;
  }
  std::cout<<corners.size()<<" points saved to "<<points_path()<<std::endl;

  return 0;
}
