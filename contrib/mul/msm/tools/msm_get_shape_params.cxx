//:
// \file
// \brief Tool to compute shape parameters for each set of points
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
//: File to containing shape model
shape_model_path: shape_model.bfs

output_path: shape_params.txt

//: Directory to save best fit points
//  If parameter is not set then do not write best fit points.
out_points_dir: /home/bestfit_points/

image_dir: /home/images/
points_dir: /home/points/
images: {
  image1.pts : image1.jpg
  image2.pts : image2.jpg
}
<END FILE>

Note: You can use the same file as was used to build the model, defining the output
      path using the -o command line option.
*/

void print_usage()
{
  std::cout << "msm_get_shape_params -p param_file\n"
           << "Compute shape parameters for each set of points.\n"
           << "Loads in named model and each set of points\n"
           << "Fits model to each set of points and saves\n"
           << "pose and shape parameters to a text file.\n"
           << "One row per shape.\n"
           << "Also save best fit points for every image.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: File containing the shape model
  std::string shape_model_path;

  //: File to save parameters to
  std::string output_path;

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

  shape_model_path=props.get_required_property("shape_model_path");

  image_dir=props.get_optional_property("image_dir","./");
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
  vul_arg<std::string> shape_model_path("-s","Shape model path (over-riding param file)");
  vul_arg<std::string> out_path("-o","Output path (over-riding param file)");
  vul_arg<bool> no_pose("-no_pose","Don't display pose",false);
  vul_arg<bool> rel_params("-rel_p","Record params[i]/sd[i]",false);
  vul_arg<bool> use_pts_name("-use_pts_name","Include name of points at beginning of the line",false);
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

  if (out_path()!="") params.output_path = out_path();
  if (shape_model_path()!="") params.shape_model_path = shape_model_path();

  msm_shape_model shape_model;

  if (!vsl_quick_file_load(shape_model,params.shape_model_path))
  {
    std::cerr<<"Failed to load shape model from "
            <<params.shape_model_path<<'\n';
    return 2;
  }

  msm_shape_instance sm_instance(shape_model);

  // Load in all the shapes
  std::vector<msm_points> shapes;
  load_shapes(params.points_dir,params.points_names,shapes);

  // Open the text file for output
  std::ofstream ofs(params.output_path.c_str());
  if (!ofs)
  {
    std::cerr<<"Failed to open "<<params.output_path <<" for output.\n";
    return 1;
  }

  bool write_bestfitpts = true;
  if (params.out_points_dir == "")
    write_bestfitpts = false;
  else
    std::cout << "Write best fit points to " << params.out_points_dir << std::endl;

  vnl_vector<double> sd = shape_model.mode_var();
  for (double & i : sd) i=std::sqrt(i);
  mbl_stats_1d mahal_stats;

  for (unsigned i=0;i<shapes.size();++i)
  {
    if (shapes[i].size()==0) continue;
    sm_instance.fit_to_points(shapes[i]);

    if (use_pts_name()) ofs<<params.points_names[i]<<' ';

    if (!no_pose())
    {
      // Write pose parameters
      for (double j : sm_instance.pose())
        ofs<<j<<' ';
    }

    // Write shape parameters
    vnl_vector<double> b = sm_instance.params();
    double M=0.0;  // Mahalanobis distance
    for (unsigned j=0;j<b.size();++j)
    {
      if (rel_params())
        ofs<<b[j]/sd[j]<<' ';
      else
        ofs<<b[j]<<' ';
      M += b[j]*b[j]/(sd[j]*sd[j]);
    }
    mahal_stats.obs(M);

    ofs<<std::endl;

    // Write best fit points
    if (write_bestfitpts)
    {
      // Check that the directory exists (points_name may include a dir)
      std::string out_pts_dir = vul_file::dirname(params.out_points_dir+"/"+params.points_names[i]);
      if (!vul_file::is_directory(out_pts_dir))
      {
        std::cout<<"Directory "<<out_pts_dir
                <<" does not exist. Creating it."<<std::endl;
        if (!vul_file::make_directory_path(out_pts_dir))
        {
          std::cerr<<"Unable to create it.\n";
          return 12;
        }
      }
      sm_instance.points().write_text_file(params.out_points_dir+"/"+params.points_names[i]);
    }
  }
  ofs.close();

  std::cout<<"Statistics of Mahalanobis distance: "<<mahal_stats<<std::endl;

  std::cout<<"Wrote parameters for "<<shapes.size()<<" shapes to "<<params.output_path<<std::endl;
  if (use_pts_name()) std::cout<<"Each line starts with filename."<<std::endl;
  if (!no_pose()) std::cout<<"First "<<sm_instance.pose().size()<<" values are pose."<<std::endl;
  std::cout<<"Next "<<sm_instance.params().size()<<" values are shape params";
  if (rel_params()) std::cout<<" in units of SD.";
  std::cout<<std::endl;

  return 0;
}
