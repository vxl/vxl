#include <string>
#include <iostream>
#include <cstdlib>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

//assumes that the base image is in the dir and the dir is
//named for the image without the extension.
static bool generate_rset(std::string const& dirpath,
                          std::string const& base_image_extension,
                          const unsigned nlevels)
{
  std::string slash;
  //generate the temporary dir
#ifdef _WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  //to get the filename, strip the extension off the base image file name
  std::string dir = vul_file::basename(dirpath);
  std::string base_image_path = dirpath + slash + dir + "_0." + base_image_extension;
  vil_image_resource_sptr base_image
    = vil_load_image_resource(base_image_path.c_str());
  if (!base_image)
    return false;

  vil_pyramid_image_resource_sptr pir =
    vil_new_pyramid_image_list_from_base(dirpath.c_str(), base_image, nlevels,
                                         false, "tiff", dir.c_str());
  return (bool)pir;
}

int main(int argc,char * argv[])
{
    if (argc<4)
    {
      std::cout<<"Usage : generate_rset.exe base_image_dir base_image_extension nlevels\n";
      return -1;
    }
    else
    {
      std::string base_image_dir(argv[1]);
      std::string base_image_extension(argv[2]);
      unsigned nlevels = std::atoi(argv[3]);

      std::cout << base_image_dir << std::endl
               << base_image_extension << std::endl
               << nlevels << std::endl;
      if (nlevels<2)
      {
        std::cout << "Must have at least 2 levels\n";
        return 0;
      }
      if (!generate_rset(base_image_dir, base_image_extension, nlevels))
    {
        std::cout << "Generate R Set failed\n";
        return -1;
      }
      return 0;
    }
}
