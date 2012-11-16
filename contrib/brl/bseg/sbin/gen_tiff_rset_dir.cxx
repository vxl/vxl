// This is brl/bseg/sbin/gen_tiff_rset_dir.cxx

#include <vcl_string.h>
#include <vcl_cstdlib.h> // for std::system()
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
  vcl_string file_base = vul_file::strip_directory(dirpath);

  vcl_string base_image_path = dirpath + slash + file_base + "_0." + base_image_extension;

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
  vcl_string slash;
#ifdef VCL_WIN32
  slash =  "\\";
#else
  slash = "/";
#endif

  if (argc<4)
  {
    vcl_cout<<"Usage : gen_tiff_rset_dir.exe base_dir base_image_extension nlevels\n";
    return -1;
  }
  else
  {
    vcl_string base_dir(argv[1]);
    vcl_string base_image_extension(argv[2]);
    unsigned nlevels = vcl_atoi(argv[3]);

    vcl_cout << base_dir << vcl_endl
             << base_image_extension << vcl_endl
             << nlevels << vcl_endl;

    if (nlevels<2)
    {
      vcl_cout << "Must have at least 2 levels\n";
      return 0;
    }

    vcl_string file_template = base_dir + slash + "*." +base_image_extension;

    for (vul_file_iterator fn=file_template; fn; ++fn)
    {
      vcl_string dir_plus_filename = fn();
      vcl_string filename = vul_file::strip_directory(dir_plus_filename);
      vcl_string filebase = vul_file::strip_extension(filename);
      vcl_string new_dir  = vul_file::strip_extension(dir_plus_filename);

      bool success = vul_file::make_directory(new_dir);

      vcl_string dir = vul_file::dirname(dir_plus_filename);

      vcl_string pyramid_dir = dir + slash + filebase;

      vcl_string new_filename = base_dir + slash + filebase + slash +
                                filebase + "_0." + base_image_extension;

      vcl_string command = "move " + dir_plus_filename + " " + new_filename;

      if (vcl_system(command.c_str()) >= 0) {    // execute file move & rename
        success = success && vul_file::change_directory(pyramid_dir); // change to pyramid dir
        vcl_cout << "Creating pyramid for: " << new_filename << vcl_endl;
        if (!generate_rset(pyramid_dir, base_image_extension, nlevels))
        {
          vcl_cout << "Generate R Set failed for file " << pyramid_dir << vcl_endl;
          return -1;
        }
        vul_file::change_directory(base_dir);  // back to base dir
      }
      else
      {
        vcl_cout << "Command execution failed for\n  " << command << vcl_endl;
      }
      if (!success)
        vcl_cout << "Failed to mkdir or to chdir\n";
    }
  }
  return 0;
}
