//:
// \file
// \brief Example of using rgb views and automatic conversion.
// \author Ian Scott

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_print.h>

int main(int argc, char** argv)
{
  if (argc < 3)
  {
    vcl_cerr << "Usage: vil2_file_example4 rgb_file grey_file" <<vcl_endl;
    return 3;
  }
  
  vcl_cout<<"Load " << argv[1] << " into an image data object" << vcl_endl;


  // This is how we quickly load an rgb image view.

  vil2_image_view<vil_rgb<vxl_byte> > b_im = vil2_load(argv[1]);

  if (!b_im)
  {
    vcl_cerr << "Couldn't load " << argv[1] <<vcl_endl;
    return 3;
  }

  
  vil2_print_all(vcl_cout, b_im);

  // Note that if we try assign an image to a non compatible view type,
  // then the view will be set to empty.

  vcl_cout<<"\n\n\nTry to load greyscale file " << argv[2] << " into an rgb image object.\nIt should fail.\n";

  b_im = vil2_load(argv[2]);


  vil2_print_all(vcl_cout, b_im);

  return 0;
}
