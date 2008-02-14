#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vil/vil_load.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>

//assumes that the base image is in the dir and the dir is
//named for the image without the extension.
static bool generate_rset(vcl_string const& dirpath,
                          vcl_string const& base_image_extension,
                          const unsigned nlevels)
{
  vcl_string slash;
  //generate the temporary dir
#ifdef VCL_WIN32
  slash =  "\\";
#else
  slash = "/";
#endif
  //to get the filename, strip the extension off the base image file name
  vcl_string dir = vul_file::basename(dirpath);
  vcl_string base_image_path = dirpath + slash + dir + "_0." + base_image_extension;
  vil_image_resource_sptr base_image
    = vil_load_image_resource(base_image_path.c_str());
  if (!base_image)
    return false;

  vil_pyramid_image_resource_sptr pir =
    vil_new_pyramid_image_list_from_base(dirpath.c_str(), base_image, nlevels,
                                         false, "tiff", dir.c_str());
  if (!pir)
    return false;
  return true;
}

int main(int argc,char * argv[])
{
    if (argc<4)
    {
      vcl_cout<<"Usage : generate_rset.exe base_image_dir base_image_extension nlevels\n";
      return -1;
    }
    else
    {
      vcl_string base_image_dir(argv[1]);
      vcl_string base_image_extension(argv[2]);
      unsigned nlevels = vcl_atoi(argv[3]);

      vcl_cout << base_image_dir << vcl_endl;
      vcl_cout << base_image_extension << vcl_endl;
      vcl_cout << nlevels << vcl_endl;
      if (nlevels<2)
      {
        vcl_cout << "Must have at least 2 levels\n";
        return 0;
      }
      if (!generate_rset(base_image_dir, base_image_extension, nlevels))
    {
        vcl_cout << "Generate R Set failed\n";
        return -1;
      }
      return 0;
    }
}
