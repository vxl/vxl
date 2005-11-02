//:
// \file
// \brief Example of creating, processing and using an image data object
// \author Ian Scott

#include <vcl_iostream.h>
#include <vil/vil_load.h>
#include <vil/vil_crop.h>
#include <vil/vil_image_view.h>
#include <vil/vil_print.h>

//: Analyse an image file without reading the pixels.
int main(int argc, char** argv)
{
  if (argc < 2)
  {
    vcl_cerr << "Specify an image filename\n";
    return 3;
  }

  vcl_cout<<"Load " << argv[1] << " into an image data object\n";


  // This is how we initialise an image resource object.

  vil_image_resource_sptr data = vil_load_image_resource(argv[1]);

  if (!data)
  {
    vcl_cerr << "Couldn't load " << argv[1] <<vcl_endl;
    return 3;
  }

  vcl_cout << "Loaded " << argv[1] <<vcl_endl;

  vcl_cout<<"Image is:" <<
    "size: " << data->ni() << 'x' << data->nj() << '\n' <<
    "planes: " << data->nplanes() << '\n' <<
    "pixel type: " << data->pixel_format() << vcl_endl;

  return 0;
}
