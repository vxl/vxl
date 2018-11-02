//:
// \file
// \brief Tool to equally space some points along curves.
// \author Tim Cootes

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <msm/msm_equally_space.h>
#include <mbl/mbl_stats_1d.h>
/*
Parameter file format:
<START FILE>
// Curves file (or curves themselves) defining which points to be moved.
// All internal points on a curve are slid along the cubic bezier through
// the points so that they are equally spaced.
curves: equal_spacing.crvs

//: Directory to new points
out_points_dir: /home/equal_spaced_points/

//: Smallest separation of points on bezier curve
min_bez_sep: 0.5

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
  std::cout << "msm_equally_space_points -p param_file [-c curves][-od output_dir]\n"
           << "For each curve, fit a cubic bezier, then move internal\n"
           << "points so that they are equally spaced along the curve.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Curves to define which points to equally space.
  msm_curves curves;

  //: Directory containing images
  std::string image_dir;

  //: Directory containing points
  std::string points_dir;

  //: Directory to save best fit points
  std::string out_points_dir;

  //: Smallest separation of points on bezier curve
  double min_bez_sep;


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

  min_bez_sep=vul_string_atof(props.get_optional_property("min_bez_sep","0.5"));

  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");

  out_points_dir=props.get_optional_property("out_points_dir","");

  std::string curves_data = props.get_optional_property("curves","-");
  curves.parse_or_load(curves_data);

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

/*
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
*/

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> curves_path("-c","Curves path");
  vul_arg<std::string> output_dir("-od","Output directory");
  vul_arg<bool> verbose("-v","Verbose output",false);
  vul_arg_parse(argc,argv);

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

  if (output_dir()!="") params.out_points_dir = output_dir();
  if (curves_path()!="") params.curves.parse_or_load(curves_path());

  if (params.points_dir==params.out_points_dir)
  {
    std::cerr<<"Input and output points directory the same ("<<params.points_dir<<")"<<std::endl;
    std::cerr<<"They must be different to avoid over-writing the points."<<std::endl;
    return 2;
  }

  msm_points points,old_points;

  // Identify which points may move and check for duplicates
  std::vector<bool> may_move(params.curves.max_index()+1);
  for (unsigned c=0;c<params.curves.size();++c)
  {
    unsigned nj=params.curves[c].size();
    if (!params.curves[c].open()) nj++;
    for (unsigned j=1;j+1<nj;++j)
    {
      if (may_move[params.curves[c][j]])
        std::cerr<<"Warning. Point "<<params.curves[c][j]<<" in more than one curve."<<std::endl;
      may_move[params.curves[c][j]]=true;
    }
  }
  unsigned n_moved=0;
  for (auto && j : may_move) if (j) n_moved++;

  std::cout<<"Number of points which may be moved: "<<n_moved<<std::endl;
  std::cout<<"Writing new points to "<<params.out_points_dir<<std::endl;

  if (verbose())
    std::cout<<"Movement per file:"<<std::endl;

  for (unsigned i=0;i<params.points_names.size();++i)
  {
    std::string path = params.points_dir+"/"+params.points_names[i];
    if (!points.read_text_file(path))
    {
      std::cerr<<"Failed to load points from "<<path<<std::endl;
      return 1;
    }
    old_points=points;

    msm_equally_space(points,params.curves,params.min_bez_sep);

    std::string new_path = params.out_points_dir+"/"+params.points_names[i];
    if (!points.write_text_file(new_path))
    {
      std::cerr<<"Failed to write points to "<<new_path<<std::endl;
      return 1;
    }

    // Compute movement
    mbl_stats_1d stats;
    for (unsigned j=0;j<points.size();++j)
    {
      if (!may_move[j]) continue;
      double move=(points[j]-old_points[j]).length();
      stats.obs(move);
    }
    if (verbose())
      std::cout<<params.points_names[i]<<" Mean: "<<stats.mean()<<" Max: "<<stats.max()<<std::endl;
  }

  return 0;
}
