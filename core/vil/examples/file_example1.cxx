//:
// \file
// \brief Example of creating and using an image data object
// \author Ian Scott

#include <vcl_iostream.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_image_view.h>
#include <vil2/vil2_print.h>

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    vcl_cerr << "Specify an image filename\n";
    return 3;
  }

  vcl_cout<<"Load " << argv[1] << " into an image data object\n";


  // This is how we initialise an image data object.

  vil2_image_resource_sptr data = vil2_load_image_resource(argv[1]);

  if (!data)
  {
    vcl_cerr << "Couldn't load " << argv[1] <<vcl_endl;
    return 3;
  }

  // This is how we get some image pixels from it.

  vil2_image_view<unsigned char> uc_view = data->get_view(0,data->ni(),0, data->nj());

  vil2_print_all(vcl_cout,uc_view);

  return 0;
}
