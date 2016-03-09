#include <mvl2/mvl2_image_format_plugin.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

int main(int argc, char **argv)
{
  if (argc<3)
    {
    std::cout << "usage : " << argv[0] << " src_image dest_image" << std::endl;
    std::cout << "Loads from file src_image and saves to file dest_image"
        << std::endl;
    std::cout << "The source image can be extracted from an AVI or a SEQUENCE"
        << std::endl;
    std::cout << "by concatenating the frame number to the filename before"
        << std::endl;
    std::cout << "the extension." << std::endl;
    std::cout << "Example : " << argv[0] << " ./test_3.avi img.jpg" << std::endl;
    std::cout << "copies the third frame of the AVI file ./test.avi to the"
        << std::endl;
    std::cout << "image file img.jpg" << std::endl;
    return -1;
    }

  vil_image_view_base_sptr img;

  vil_image_resource_plugin::register_plugin(
    new mvl2_image_format_plugin());


  img=vil_load(argv[1]);

  if (!img || img->size()==0)
    {
    std::cout << "Unable to load source image from " << argv[1] << std::endl;
    return -1;
    }

  if (!vil_save(*img,argv[2]))
    {
    std::cerr << "Unable to save result image to " << argv[2] << std::endl;
    return -1;
    }

  vil_image_resource_plugin::delete_all_plugins();

  return 0;
}
