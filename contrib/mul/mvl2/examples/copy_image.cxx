#include <mvl2/mvl2_image_format_plugin.h>
#include <vil2/vil2_load.h>
#include <vil2/vil2_save.h>

int main(int argc, char **argv)
{
  if (argc<3)
    {
    vcl_cout << "usage : " << argv[0] << " src_image dest_image" << vcl_endl;
    vcl_cout << "Loads from file src_image and saves to file dest_image" 
        << vcl_endl;
    vcl_cout << "The source image can be extracted from an AVI or a SEQUENCE"
        << vcl_endl;
    vcl_cout << "by concatenating the frame number to the filename before"
        << vcl_endl;
    vcl_cout << "the extension." << vcl_endl;
    vcl_cout << "Example : " << argv[0] << " ./test_3.avi img.jpg" << vcl_endl;
    vcl_cout << "copies the third frame of the AVI file ./test.avi to the" 
        << vcl_endl;
    vcl_cout << "image file img.jpg" << vcl_endl;
    return -1;
    }

  vil2_image_view_base_sptr img;

  vil2_image_resource_plugin::register_plugin(
    new mvl2_image_format_plugin());


  img=vil2_load(argv[1]);
 
  if (!img || img->size()==0)
    {
    vcl_cout << "Unable to load source image from " << argv[1] << vcl_endl; 
    return -1;
    }

  if (!vil2_save(*img,argv[2]))
    {
    vcl_cerr << "Unable to save result image to " << argv[2] << vcl_endl;
    return -1;
    }
  
  vil2_image_resource_plugin::delete_all_plugins();

  return 0;
}
