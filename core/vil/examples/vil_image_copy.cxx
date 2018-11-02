//:
// \file
// \brief Example of loading an image and saving it again (to test image loaders)
// \author Tim Cootes

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>

int main(int argc, char** argv)
{
  if (argc!=3)
  {
    std::cout<<"vil_image_copy  src_image dest_image\n"
            <<"Loads from file src_image, saves to file dest_image\n";
    return 0;
  }

  vil_image_view<vxl_byte> src_im = vil_load(argv[1]);
  if (src_im.size()==0)
  {
    std::cout<<"Unable to load source image from "<<argv[1]<<std::endl;
    return 1;
  }

  std::cout<<"Loaded image of size "<<src_im.ni()<<" x "<<src_im.nj()<<std::endl;

  if (!vil_save(src_im, argv[2]))
  {
    std::cerr<<"Unable to save result image to "<<argv[2]<<std::endl;
    return 1;
  }

  std::cout<<"Saved image to "<<argv[2]<<std::endl;

  return 0;
}
