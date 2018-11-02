//:
// \file
// \brief Example of loading an image, rotating + saving
// \author dac

#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_rotate.h>

int main(int argc, char** argv)
{
  if (argc!=4)
  {
    std::cout<<"vil_rotate_image src_image dest_image theta\n"
            <<"Loads from src_image\n"
            <<"rotates by angle theta (degrees)\n"
            <<"saves result to dest_image\n";
    return 0;
  }

  vil_image_view<vxl_byte> src_im = vil_load(argv[1]);
  if (src_im.size()==0)
  {
    std::cout<<"Unable to load source image from "<<argv[1]<<std::endl;
    return 1;
  }

  std::cout<<"Loaded image of size "<<src_im.ni()<<" x "<<src_im.nj()
          <<" from "<<argv[1]<<std::endl;


  double theta = std::atof(argv[3]);

  // rotate the image
  vil_image_view<vxl_byte> dest_im;
  vil_rotate_image( src_im, dest_im, theta );

  std::cout<<"src_im = "<<src_im<<std::endl
          <<"dest_im = "<<dest_im<<std::endl;

  if (!vil_save(dest_im, argv[2]))
  {
    std::cerr<<"Unable to save result image to "<<argv[2]<<std::endl;
    return 1;
  }

  std::cout<<"Saved rotated image of size "<<dest_im.ni()<<" x "<<dest_im.nj()
          <<" to "<<argv[2]<<std::endl;

  return 0;
}
