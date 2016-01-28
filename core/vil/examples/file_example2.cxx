// :
// \file
// \brief Example of creating, processing and using an image data object
// \author Ian Scott

#include <vcl_iostream.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

int main(int argc, char* * argv)
{
  if( argc < 2 )
    {
    vcl_cerr << "Specify an image filename\n";
    return 3;
    }

  vcl_cout << "Load " << argv[1] << " into an image data object\n";

  // This is how we initialise an image data object.

  vil_image_resource_sptr data = vil_load_image_resource(argv[1]);

  if( !data )
    {
    vcl_cerr << "Couldn't load " << argv[1] << vcl_endl;
    return 3;
    }

  vcl_cout << "Crop the image by 1 pixel around all sides.\n";

  // We can apply some operation to it.

  vil_image_resource_sptr cropped_data = vil_crop(data, 1, data->ni() - 2, 1, data->nj() - 2);

  // And then get the image pixels from it.

  vil_image_view<unsigned char> uc_view = cropped_data->get_view(0, cropped_data->ni(), 0, cropped_data->nj() );

  vcl_cout << "Created a view of type " << uc_view.is_a() << vcl_endl;

  vil_print_all(vcl_cout, uc_view);

  return 0;
}
