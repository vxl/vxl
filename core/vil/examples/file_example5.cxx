//:
// \file
// \brief Example of creating, processing and using an image data object
// \author Ian Scott

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_load.h>

//: Analyse an image file without reading the pixels.
int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Specify an image filename\n";
    return 3;
  }

  std::cout<<"Load " << argv[1] << " into an image data object\n";


  // This is how we initialise an image resource object.

  vil_image_resource_sptr data = vil_load_image_resource(argv[1]);

  if (!data)
  {
    std::cerr << "Couldn't load " << argv[1] <<std::endl;
    return 3;
  }

  std::cout << "Loaded " << argv[1] <<std::endl;

  std::cout<<"Image is:\n"
          <<" size: " << data->ni() << 'x' << data->nj() << '\n'
          <<" planes: " << data->nplanes() << '\n'
          <<" pixel type: " << data->pixel_format() << std::endl;

  return 0;
}
