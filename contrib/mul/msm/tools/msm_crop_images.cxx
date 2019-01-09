//:
// \file
// \brief Tool to crop images to region around points
// For each input image and points, generates an output image and associated points
// Crops to a region around the points, and optionally rescales.
// Output image is saved as a byte image, with range stretched to [0,255]
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
#include <vul/vul_file.h>
#include <msm/msm_shape_model.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_add_all_loaders.h>

#include <mbl/mbl_stats_1d.h>
/*
Parameter file format:
<START FILE>

// Where to save new images
out_image_dir: cropped_images

// Where to save new points
out_points_dir: cropped_points

image_dir: /home/images/
points_dir: /home/points/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}
<END FILE>
*/

void print_usage()
{
  std::cout << "msm_crop_images -p param_file\n"
          << "Tool to crop images to region around points.\n"
          << "For each input image and points, generates an output image and associated points.\n"
          << "Crops to a region around the points, and optionally rescales.\n"
          << "Output image is saved as a byte image, with range stretched to [0,255].\n"
          << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  // Where to save new images
  std::string out_image_dir;

  // Where to save new points
  std::string out_points_dir;

  //: Border width (as proportion of size)
  double border_width;

  //: Directory containing images
  std::string image_dir;

  //: Directory containing points
  std::string points_dir;

  //: Directory to save best fit points
  std::string out_points_dir;

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



  out_image_dir=props.get_optional_property("out_image_dir","./cropped_images");
  out_points_dir=props.get_optional_property("out_points_dir","./cropped_points");
  image_dir=props.get_optional_property("image_dir","./");
  border_width=vul_string_atoi(props.get_optional_property("border_width","0.1"));
  points_dir=props.get_optional_property("points_dir","./");
  output_path=props.get_optional_property("output_path",
                                          "shape_params.txt");
  out_points_dir=props.get_optional_property("out_points_dir","");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

void load_shapes(const std::string& points_dir,
                 const std::vector<std::string>& filenames,
                 std::vector<msm_points>& shapes)
{
  unsigned n=filenames.size();

  shapes.resize(n);
  for (unsigned i=0;i<n;++i)
  {
    std::string path = points_dir+"/"+filenames[i];
    if (!shapes[i].read_text_file(path))
    {
      mbl_exception_parse_error x("Failed to load points from "+path);
      mbl_exception_error(x);
    }
  }
}


int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg_parse(argc,argv);

  msm_add_all_loaders();

  if (param_path()=="")
  {
    print_usage();
    return 0;
  }

  tool_params params;
  try
  {
    params.read_from_file(param_path());
  }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }


  msm_points points;
  vimt_image_2d_of<float> image;
  unsigned n=params.image_names.size();
  for (unsigned i=0;i<n;++i)
  {
    std::string pts_path = params.points_dir+"/"+params.points_names[i];
    if (!points.read_text_file(pts_path))
    {
      std::cerr<<"Failed to load points from "+path<<std::endl;
      return 1;
    }

    std::string image_path = params.image_dir+"/"+params.image_names[i];

// load as float

    // Project points into image frame
    points.apply_transform(image.world2im());
    vgl_box_2d<double> bbox=points.bounds();

  }


  return 0;
}
