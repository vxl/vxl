//:
// \file
// \brief Tool to build a finder from a set of training images
// \author Tim Cootes
// Builds a model of patch around a given point.
// For simplicity, assumes fixed scale and orientation across all data.
// Input data file format:
// \code
// patch_builder: mfpf_norm_corr2d_builder { ni: 11 nj: 11 }
// pt_index: 31 // Which point to use
// res_level: 1
// model_path: patch_model.bfs
//
// image_dir: ../images/
// points_dir: ../points/
// images: {
//   image1.pts : image1.jpg
//   image2.pts : image2.jpg
// }
// \endcode

#include <iostream>
#include <cmath>
#include <sstream>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>

#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#include <vsl/vsl_quick_file.h>
#include <vsl/vsl_binary_loader.h>

#include <mfpf/mfpf_point_finder.h>
#include <mfpf/mfpf_point_finder_builder.h>
#include <mfpf/mfpf_add_all_loaders.h>

#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

#include <msm/msm_points.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Structure to hold parameters
struct tool_params
{
  //: Path to which to save the model
  std::string model_path;

  //: Resolution level at which to build model
  unsigned res_level;

  //: Index of point to use to train model
  unsigned pt_index;

  //: Object to build patch model
  std::unique_ptr<mfpf_point_finder_builder> patch_builder;

  //: Image directory
  std::string image_dir;

  //: Points directory
  std::string points_dir;

  //: List of image filenames
  std::vector<std::string> image_names;

  //: List of points filenames
  std::vector<std::string> points_names;

  // Constructor to set defaults
  tool_params();

  //: Parse named text file to read in data
  //  Throws a upf_exception_parse_error if fails
  void read_from_file(const std::string& path);
};

tool_params::tool_params() = default;

//: Parse named text file to read in data
//  Throws a upf_exception_parse_error if fails
void tool_params::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props;
  props = mbl_read_props_ws(ifs);

  model_path=props.get_required_property("model_path");
  image_dir=props.get_required_property("image_dir");
  points_dir=props.get_required_property("points_dir");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  res_level = vul_string_atoi(props.get_optional_property("res_level","0"));
  pt_index = vul_string_atoi(props.get_optional_property("pt_index","0"));

  std::string builder_str= props.get_required_property("patch_builder");
  std::istringstream iss(builder_str);
  patch_builder = mfpf_point_finder_builder::create_from_stream(iss);

  try {
    mbl_read_props_look_for_unused_props(
      "::read_from_file", props, mbl_read_props_type());
  }
  catch (mbl_exception_unused_props& e)
  {
    throw (mbl_exception_parse_error(e.what()));
  }
}

void print_usage()
{
  std::cout<<"mfpf_build_finder -p param_file\n"
          <<"Tool to build a finder from a set of training images\n"
          <<std::endl;
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");

  vul_arg_parse(argc,argv);

  if (param_path()=="")
  {
    print_usage();
    return 0;
  }

  mfpf_add_all_loaders();

  tool_params params;
  try { params.read_from_file(param_path()); }
  catch (mbl_exception_parse_error& e)
  {
    std::cerr<<"Error: "<<e.what()<<std::endl;
    return 1;
  }

  unsigned n_images = params.image_names.size();

  vimt_gaussian_pyramid_builder_2d<float> pyr_builder;

  params.patch_builder->set_step_size(std::pow(2.0,double(params.res_level)));
  params.patch_builder->clear(n_images);

  vgl_vector_2d<double> u(1,0);  // Default to unit scale and fixed angle

  for (unsigned i=0;i<n_images;++i)
  {
    // Load in image
    vil_image_view<vxl_byte> byte_image;
    std::string image_path = params.image_dir+"/"+params.image_names[i];
    byte_image = vil_load(image_path.c_str());
    if (byte_image.size()==0)
    {
      std::cerr<<"Failed to load in image from "<<image_path<<std::endl;
      return 2;
    }

    // Convert to float image and build pyramid
    vimt_image_2d_of<float> image;
    if (byte_image.nplanes()==1)
      vil_convert_cast(byte_image,image.image());
    else
      vil_convert_planes_to_grey(byte_image,image.image());

    vimt_image_pyramid image_pyr;
    pyr_builder.build(image_pyr,image);

    assert(int(params.res_level)<=image_pyr.hi());

    // Select relevant level from pyramid
    const auto& imageL
      = static_cast<const vimt_image_2d_of<float>&>(image_pyr(params.res_level));

    // Load in points
    msm_points points;
    std::string points_path = params.points_dir + "/"
                           + params.points_names[i];

    if (!points.read_text_file(points_path))
    {
      std::cerr<<"Failed to load points from "<<points_path<<std::endl;
      return 3;
    }

    vgl_point_2d<double> p = points[params.pt_index];

    params.patch_builder->add_example(imageL,p,u);
  }

  // Create and build a model
  mfpf_point_finder *finder = params.patch_builder->new_finder();
  params.patch_builder->build(*finder);

  std::cout<<"Finder: "<<*finder<<std::endl;

  vsl_quick_file_save(finder,params.model_path);

  vimt_image_2d_of<vxl_byte> model_image;
  finder->get_image_of_model(model_image);
  std::string patch_image_path = "./model_patch.jpg";
  if (vil_save(model_image.image(),patch_image_path.c_str()))
    std::cout<<"Saved patch image to "<<patch_image_path<<std::endl;

  delete finder;

  return 0;
}
