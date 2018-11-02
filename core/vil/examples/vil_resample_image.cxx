//:
// \file
// \brief Example of loading an image, resampling + saving
// \author dac

#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_resample_bicub.h>

int main(int argc, char** argv)
{
  if (argc!=5)
  {
    std::cout<<"vil_resample_image src_image dest_image n1 n2\n"
            <<"Loads from src_image\n"
            <<"resamples to size n1*n2\n"
            <<"saves result to dest_image\n";
    return 0;
  }

  vil_image_view<vxl_byte> src_im = vil_load(argv[1]);
  if (src_im.size()==0)
  {
    std::cout<<"Unable to load source image from "<<argv[1]<<std::endl;
    return 1;
  }

  std::cout<<"Loaded image of size "<<src_im.ni()<<" x "<<src_im.nj()<<std::endl;

  int n1= std::atoi(argv[3]);
  int n2= std::atoi(argv[4]);

  // resample the image
  vil_image_view<vxl_byte> dest_im;
  vil_resample_bicub( src_im, dest_im, n1, n2 ); // or vil_resample_bilin()

  std::cout<<"src_im= "<<src_im<<std::endl
          <<"dest_im= "<<dest_im<<std::endl;

  if (!vil_save(dest_im, argv[2]))
  {
    std::cerr<<"Unable to save result image to "<<argv[2]<<std::endl;
    return 1;
  }

  std::cout<<"Saved image to "<<argv[2]<<std::endl;

  return 0;
}
