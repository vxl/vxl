//:
// \file
// \brief Tool to test a finder on a set of annotated images
// \author Tim Cootes
// Loads a model of patch around a given point (from mfpf_build_finder tool)
// For simplicity, assumes fixed scale and orientation across all data.
// Uses the patch model to search the images and measures how good best
// fit is.
// Input data file format:
// \code
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
#include <mfpf/mfpf_sort_matches.h>

#include <vil/vil_load.h>
#include <vil/vil_convert.h>
#include <vimt/vimt_image_2d_of.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

#include <msm/msm_points.h>
#include <mbl/mbl_stats_1d.h>
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
  std::cout<<"mfpf_test_finder -p param_file\n"
          <<"Tool to test a finder from a set of annotated images\n"
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

  vgl_vector_2d<double> u(1,0);  // Default to unit scale and fixed angle

  mfpf_point_finder *finder = nullptr;
  vsl_quick_file_load(finder,params.model_path);
  std::cout<<"Finder: "<<*finder<<std::endl;

  finder->set_search_area(10,10);

  mbl_stats_1d d_stats,rank_stats,min_d_stats;

  std::vector<mfpf_pose> poses;
  std::vector<double> fits;

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
    std::string points_path = params.points_dir+"/"
                              +params.points_names[i];

    if (!points.read_text_file(points_path))
    {
      std::cerr<<"Failed to load points from "<<points_path<<std::endl;
      return 3;
    }

    vgl_point_2d<double> p = points[params.pt_index];

    // Search image and evaluate match
    vgl_point_2d<double> new_p;
    vgl_vector_2d<double> new_u;

    // Slight fudge since it searches around the true point
    /* double fit = */ finder->search(imageL,p,u,new_p,new_u);

    d_stats.obs((new_p-p).length());

    poses.resize(0); fits.resize(0);
    finder->multi_search(imageL,p,u,poses,fits);
    mfpf_sort_matches(poses,fits);

    // Find the index of the closest point to p
    if (poses.size()>0)
    {
      double min_d = (p-poses[0].p()).length();
      unsigned best_j = 0;
      for (unsigned j=1;j<poses.size();++j)
      {
        double d = (p-poses[j].p()).length();
        if (d<min_d) { min_d=d; best_j=j; }
      }
      rank_stats.obs(best_j);
      min_d_stats.obs(min_d);
    }
  }

  std::cout<<"d_stats: "<<d_stats<<std::endl
          <<"rank_stats: "<<rank_stats<<std::endl
          <<"min_d_stats: "<<min_d_stats<<std::endl;

  delete finder;

  return 0;
}
