//:
// \file
// \brief Example of creating, processing and using an image data object
// \author Ian Scott

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>


int main(int argc, char** argv)
{
  if (argc < 2)
  {
    std::cerr << "Specify an image filename\n";
    return 3;
  }

  std::cout<<"Load " << argv[1] << " into an image data object\n";


  // This is how we quickly load an image view.

  vil_image_view<vxl_byte> b_im = vil_load(argv[1]);

  if (!b_im)
  {
    std::cerr << "Couldn't load " << argv[1] <<std::endl;
    return 3;
  }

  vil_print_all(std::cout, b_im);

  // We can apply some operation to it.

  // And then we save it.

  vil_save(b_im, "test.pbm");


  return 0;
}
