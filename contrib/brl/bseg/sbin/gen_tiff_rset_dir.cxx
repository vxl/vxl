// This is brl/bseg/sbin/gen_tiff_rset_dir.cxx

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

#include <vul/vul_file_iterator.h>

// A new utility to make rset directories containing rsets of all the base images
//   in this directory
// Does NOT assume that the base image is in the dir and the dir is
//   named for the image without the extension.
// We must create the directory to hold each rset, move the base image to that
//   directory, and add "_0" to the end of the base image ID.

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
  std::string file_base = vul_file::strip_directory(dirpath);

  std::string base_image_path = dirpath + slash + file_base + "_0." + base_image_extension;

  vil_image_resource_sptr base_image = vil_load_image_resource(base_image_path.c_str());

  if (!base_image)
    return false;

  vil_pyramid_image_resource_sptr pir =
    vil_new_pyramid_image_list_from_base(dirpath.c_str(), base_image, nlevels,
                                         false, "tiff", dir.c_str());
  return (bool)pir;
}

// -------------------------------------------------------------------------------------
int main(int argc,char * argv[])
{
  std::string slash;
#ifdef _WIN32
  slash =  "\\";
#else
  slash = "/";
#endif

  if (argc<4)
  {
    std::cout<<"Usage : gen_tiff_rset_dir.exe base_dir base_image_extension nlevels\n";
    return -1;
  }
  else
  {
    std::string base_dir(argv[1]);
    std::string base_image_extension(argv[2]);
    unsigned nlevels = std::atoi(argv[3]);

    std::cout << base_dir << std::endl
             << base_image_extension << std::endl
             << nlevels << std::endl;

    if (nlevels<2)
    {
      std::cout << "Must have at least 2 levels\n";
      return 0;
    }

    std::string file_template = base_dir + slash + "*." +base_image_extension;

    for (vul_file_iterator fn=file_template; fn; ++fn)
    {
      std::string dir_plus_filename = fn();
      std::string filename = vul_file::strip_directory(dir_plus_filename);
      std::string filebase = vul_file::strip_extension(filename);
      std::string new_dir  = vul_file::strip_extension(dir_plus_filename);

      bool success = vul_file::make_directory(new_dir);

      std::string dir = vul_file::dirname(dir_plus_filename);

      std::string pyramid_dir = dir + slash + filebase;

      std::string new_filename = base_dir + slash + filebase + slash +
                                filebase + "_0." + base_image_extension;

      std::string command = "move " + dir_plus_filename + " " + new_filename;

      if (std::system(command.c_str()) >= 0) {    // execute file move & rename
        success = success && vul_file::change_directory(pyramid_dir); // change to pyramid dir
        std::cout << "Creating pyramid for: " << new_filename << std::endl;
        if (!generate_rset(pyramid_dir, base_image_extension, nlevels))
        {
          std::cout << "Generate R Set failed for file " << pyramid_dir << std::endl;
          return -1;
        }
        vul_file::change_directory(base_dir);  // back to base dir
      }
      else
      {
        std::cout << "Command execution failed for\n  " << command << std::endl;
      }
      if (!success)
        std::cout << "Failed to mkdir or to chdir\n";
    }
  }
  return 0;
}
