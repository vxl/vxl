//:
// \file
// \brief Example of creating, processing and using an image data object
// \author Ian Scott

#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_print.h>


int main(int argc, char** argv)
{
  if (argc < 2)
  {
    vcl_cerr << "Specify an image filename" <<vcl_endl;
    return 3;
  }
  
  vcl_cout<<"Load " << argv[1] << " into an image data object" << vcl_endl;


  // This is how we quickly load an image view.

  vil2_image_view<vxl_byte> b_im = vil2_load(argv[1]);

  if (!b_im)
  {
    vcl_cerr << "Couldn't load " << argv[1] <<vcl_endl;
    return 3;
  }

  
  

  vil2_print_all(vcl_cout, b_im);

  // We can apply some operation to it.

  // And then we save it.

  vil2_save(b_im, "test.pbm");


  return 0;
}
