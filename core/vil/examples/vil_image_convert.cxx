//:
// \file
// \brief Example of loading an image and saving it again
// It differs from vil_image_copy by not loading the image into a view
// \author Gehua

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>

int main(int argc, char** argv)
{
  if (argc<2)
  {
    std::cout<<"vil_image_copy  src_image dest_image\n"
            <<"Loads from file src_image, saves to file dest_image\n";
    return 0;
  }

  vil_image_resource_sptr src_im = vil_load_image_resource(argv[1]);
  if ( !src_im )
  {
    std::cout<<"Unable to load source image from "<<argv[1]<<std::endl;
    return 1;
  }

  // print information
  std::cout << "Loaded image " << argv[1] << '\n'
           << "Size: "<<src_im->ni()<<" x "<<src_im->nj()
           << "      Planes: " << src_im->nplanes() << '\n'
           << "Pixel format: " << src_im->pixel_format()
           << std::endl;

  if (argc==2)
  {
    return 0;
  }

  if (!vil_save_image_resource(src_im, argv[2]))
  {
    std::cerr<<"Unable to save result image to "<<argv[2]<<std::endl;
    return 1;
  }

  std::cout << "Saved image to " << argv[2] << std::endl;

  return 0;
}
