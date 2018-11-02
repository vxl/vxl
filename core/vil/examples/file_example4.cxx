//:
// \file
// \brief Example of using rgb views and automatic conversion.
// \author Ian Scott

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    std::cerr << "Usage: vil_file_example4 rgb_file grey_file\n";
    return 3;
  }

  std::cout<<"Load " << argv[1] << " into an image data object\n";


  // This is how we quickly load an rgb image view.

  vil_image_view<vil_rgb<vxl_byte> > b_im = vil_load(argv[1]);

  if (!b_im)
  {
    std::cerr << "Couldn't load " << argv[1] <<std::endl;
    return 3;
  }

  vil_print_all(std::cout, b_im);

  // Note that if we try assign an image to a non compatible view type,
  // then the view will be set to empty.

  std::cout<<"\n\n\nTry to load greyscale file " << argv[2] << " into an rgb image object.\nIt should fail.\n";

  b_im = vil_load(argv[2]);


  vil_print_all(std::cout, b_im);

  return 0;
}
