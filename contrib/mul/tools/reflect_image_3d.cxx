// This is mul/tools/reflect_image_3d.cxx

#include <iostream>
#include <exception>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vil3d/vil3d_reflect.h>
#include <vimt3d/vimt3d_add_all_loaders.h>
#include <vimt3d/vimt3d_transform_3d.h>
#include <vimt3d/vimt3d_load.h>
#include <vimt3d/vimt3d_reflect.h>
#include <vimt3d/vimt3d_save.h>
#include <mbl/mbl_log.h>
#include <mbl/mbl_exception.h>


//=========================================================================
// Static function to create a static logger when first required
//=========================================================================
static mbl_logger& logger()
{
  static mbl_logger l("mul.tools.reflect_image_3d");
  return l;
}


//=========================================================================
// Main function
//=========================================================================
int main2(int argc, char *argv[])
{
  // Parse command line arguments
  vul_arg_base::set_help_description(
    "DESCRIPTION:\n"
    "Reflect an image along one of the Cartesian axes (X, Y or Z).\n"
    "More precisely, reflect the image through a plane orthogonal to the specified axis.\n"
    "The default is to reflect about the world origin.\n"
    "Alternatively, you can specify to reflect about the image centre.\n"
    "This program modifies the image in both of the following 2 ways:\n"
    "  1. The voxel array is transposed.\n"
    "  2. The existing transform is composed with a translation so that the image centre\n"
    "     is reflected through the world origin.\n"
    "Step 2 is not performed if you specify the -c (image_centre) option.\n"
    "The output image is written in v3i format (vimt3d_image_3d_of<float>).\n"
    "NB. Default units are metres.\n"
  );
  vul_arg<std::string> src_file(nullptr, "Input image file");
  vul_arg<std::string> dst_file(nullptr, "Output image file");
  vul_arg<std::string> axis("-a", "Axis along which to reflect, i.e. X means reflect in X direction (through YZ plane)", "X");
  vul_arg<bool> image_centre("-c", "Reflect about image centre if set, otherwise about world origin", false);
  vul_arg<bool> use_mm("-mm", "World coords in units of mm", false);
  vul_arg_parse(argc, argv);

  MBL_LOG(INFO, logger(), "Program arguments:");
  MBL_LOG(INFO, logger(), "  src_file: " << src_file());
  MBL_LOG(INFO, logger(), "  dst_file: " << dst_file());
  MBL_LOG(INFO, logger(), "  axis    : " << axis());
  MBL_LOG(INFO, logger(), "  image_centre  : " << (image_centre.set() ? "image centre" : "world"));

  // Validate axis argument
  if (axis()!="X" && axis()!="x" && axis()!="Y" && axis()!="y" && axis()!="Z" && axis()!="z")
  {
    MBL_LOG(ERR, logger(), "-a option must specify X, x, Y, y, Z or z");
    throw mbl_exception_abort("-a option must specify X, x, Y, y, Z or z");
  }

  // Add all loaders
  vimt3d_add_all_loaders();

  // Load image
  if (!vul_file::exists(src_file()))
  {
    MBL_LOG(ERR, logger(), "input image file does not exist: " << src_file());
    throw mbl_exception_abort("input image file does not exist: " + src_file());
  }
  vimt3d_image_3d_of<float> img;
  vimt3d_load(src_file(), img, use_mm());
  MBL_LOG(INFO, logger(), "Loaded image file: " << src_file());

  if (axis()=="X" || axis()=="x")
  {
    // Reflect the image along the X-axis
    if (image_centre())
    {
      img.image() = vil3d_reflect_i(img.image());
    }
    else
    {
      vimt3d_reflect_x(img);
    }
    MBL_LOG(INFO, logger(), "Image reflected in X direction.");
  }
  else if (axis()=="Y" || axis()=="y")
  {
    // Reflect the image along the Y-axis
    if (image_centre())
    {
      img.image() = vil3d_reflect_j(img.image());
    }
    else
    {
      vimt3d_reflect_y(img);
    }
    MBL_LOG(INFO, logger(), "Image reflected in Y direction.");
  }
  else if (axis()=="Z" || axis()=="z")
  {
    // Reflect the image along the Z-axis
    if (image_centre())
    {
      img.image() = vil3d_reflect_k(img.image());
    }
    else
    {
      vimt3d_reflect_z(img);
    }
    MBL_LOG(INFO, logger(), "Image reflected in Z direction.");
  }

  // Create any output directories if necessary
  if (!vul_file::make_directory_path(vul_file::dirname(dst_file())))
  {
    MBL_LOG(ERR, logger(), "failed to create output directory for file: " << dst_file());
    throw mbl_exception_abort("failed to create output directory for file: " + dst_file());
  }

  // Save the reflected image to the destination file
  vimt3d_save(dst_file(), img, use_mm());
  MBL_LOG(INFO, logger(), "Saved image file: " << dst_file());

  return 0;
}


//=========================================================================
// Main function with exception-handling wrapper and logging
//=========================================================================
int main(int argc, char *argv[])
{
  int retcode = 0;

  try
  {
    mbl_logger::root().load_log_config_file();
    MBL_LOG(INFO, logger(), "BEGIN");

    retcode = main2(argc, argv);
  }
  catch (const std::exception &e)
  {
    std::cerr << "ERROR: " << e.what() << '\n';
    retcode = 1;
  }
  catch (...)
  {
    std::cout << '\n'
             << "====================================\n"
             << "Caught unknown exception.\n"
             << "Ending program.\n"
             << "====================================\n" << std::endl;
    MBL_LOG(ERR, logger(), "Caught unknown exception");
    retcode = 2;
  }

  MBL_LOG(INFO, logger(), "END");
  return retcode;
}
