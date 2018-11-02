//:
// \file
// \brief Tool to load in image and point list files, then save out list of image names alone.
// Loads in list of images and points file names, and two directory strings.
// Outputs new list just containing image names.
// \author Tim Cootes

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_quick_file.h>
#include <msm/msm_shape_model.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_add_all_loaders.h>

/*
Input file format:
<START FILE>
image_dir: /home/images/
points_dir: /home/points/
images: {
  image1.pts : eg1/image1.jpg
  image2.pts : eg1/image2.jpg
}
<END FILE>

Output result:
<START FILE>
images:
{
eg1/image1.jpg
eg1/image2.jpg
}
<END FILE>

*/

void print_usage()
{
  std::cout << "msm_images_from_list -i image_list.txt -o new_list.txt\n"
           << "Loads images and point file names, and directory strings from input file.\n"
           << "Outputs new list containing only the image names\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Directory containing images
  std::string image_dir;

  //: Directory containing points
  std::string points_dir;

  //: List of image names
  std::vector<std::string> image_names;

  //: List of points file names
  std::vector<std::string> points_names;

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  void read_from_file(const std::string& path);
};

//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void tool_params::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  image_dir=props.get_optional_property("image_dir","");
  points_dir=props.get_optional_property("points_dir","");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

int main(int argc, char** argv)
{
  vul_arg<std::string> in_path("-i","Input image + points file");
  vul_arg<std::string> out_path("-o","Output path");
  vul_arg_parse(argc,argv);

  if (in_path()=="" || out_path()=="")
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(in_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }


  // Open the text file for output
  std::ofstream ofs(out_path().c_str());
  if (!ofs)
  {
    std::cerr<<"Failed to open "<<out_path() <<" for output.\n";
    return 1;
  }

  ofs<<"images: {"<<std::endl;

  for (const auto & image_name : params.image_names)
  {
    ofs<<image_name<<std::endl;
  }
  ofs<<"}"<<std::endl;
  ofs.close();

  std::cout<<"Wrote image names to "<<out_path()<<std::endl;

  return 0;
}
