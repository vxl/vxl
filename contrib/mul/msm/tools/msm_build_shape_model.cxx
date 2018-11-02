//:
// \file
// \brief Tool to build a shape model from data in files.
// \author Tim Cootes
// Given a set of shapes, align them then apply PCA to build a shape model.
// Note that more sophisticated control over individual modes can be
// defined using the mode_part: { ... } options. These enable particular
// modes to only affect certain subsets of points.
// Although the resulting model will be less compact, it can be helpful
// to know that some modes are focused on particular regions.

#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <iterator>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_int_list.h>
#include <mbl/mbl_parse_string_list.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_quick_file.h>

#include <msm/msm_shape_model_builder.h>
#include <msm/msm_reflect_shape.h>
#include <msm/msm_curve.h>

#include <msm/msm_add_all_loaders.h>

/*
Parameter file format:
<START FILE>
//: Aligner for shape model
aligner: msm_similarity_aligner

//: Define how to align mean shape in reference frame
// Options: { first_shape, mean_pose }
ref_pose_source: first_shape

//: Object to apply limits to parameters
param_limiter: msm_ellipsoid_limiter { accept_prop: 0.98 }

// Maximum number of shape modes
max_modes: 99

// Proportion of shape variation to explain
var_prop: 0.95

//: Path to curves file defining parts to be used for individual modes
parts_for_modes_path: parts.crvs

//: Define which parts to be used for first modes
//  Where defined, only the points in the parts will be used to compute
//  that mode.  Thus some modes will only affect a subset of points.
//  Thus in the following, the third mode will only affect the points
//  listed in the mouth part in the curves file.
mode_part: { all all mouth eyebrows }


//: File to save model to
shape_model_path: shape_model.bfs

//: Define renumbering required under reflection
//  If defined, a reflected version of each shape is included in build
reflection_symmetry: { 7 6 5 4 3 2 1 0 }

//: When true, only use reflection. When false, use both reflection and original.
only_reflect: false


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
  std::cout << "msm_build_shape_model -p param_file\n"
           << "Builds the shape model from the supplied data.\n"
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Aligner for shape model
  std::unique_ptr<msm_aligner> aligner;

  msm_aligner::ref_pose_source ref_pose_source;

  //: Object to apply limits to parameters
  std::unique_ptr<msm_param_limiter> limiter;

  //: Maximum number of shape modes
  unsigned max_modes;

  //: Proportion of shape variation to explain
  double var_prop;

  //: Path to curves file defining parts to be used for individual modes
  std::string parts_for_modes_path;

  //: Define which parts to be used for first modes
  //  Where defined, only the points in the parts will be used
  //  to compute that mode.  Thus some modes will only affect a
  //  subset of points.
  std::vector<std::string> mode_part;

  //: Define renumbering required under reflection
  //  If defined, a reflected version of each shape is included in build
  std::vector<unsigned> reflection_symmetry;

  //: When true, only use reflection. When false, use both reflection and original.
  bool only_reflect;

  //: Directory containing images
  std::string image_dir;

  //: Directory containing points
  std::string points_dir;

  //: File to save shape model to
  std::string shape_model_path;

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

  max_modes=vul_string_atoi(props.get_optional_property("max_modes","99"));
  var_prop=vul_string_atof(props.get_optional_property("var_prop","0.95"));
  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");
  shape_model_path=props.get_optional_property("shape_model_path",
                                               "shape_aam.bfs");

  {
    std::string aligner_str
       = props.get_required_property("aligner");
    std::stringstream ss(aligner_str);
    aligner = msm_aligner::create_from_stream(ss);
  }

  std::string rps_str = props.get_optional_property("ref_pose_source","first_shape");
  if (rps_str=="first_shape") ref_pose_source=msm_aligner::first_shape;
  else
  if (rps_str=="mean_pose") ref_pose_source=msm_aligner::mean_pose;
  else
  {
    mbl_exception_parse_error x("Unknown ref_pose_source: "+rps_str);
    mbl_exception_error(x);
  }


  {
    std::string limiter_str
       = props.get_optional_property("param_limiter",
                                     "msm_ellipsoid_limiter { accept_prop: 0.98 }");
    std::stringstream ss(limiter_str);
    limiter = msm_param_limiter::create_from_stream(ss);
  }

  parts_for_modes_path=props.get_optional_property("parts_for_modes_path","");
  std::string mode_part_str=props.get_optional_property("mode_part","");
  mode_part.resize(0);
  if (mode_part_str!="")
    mbl_parse_string_list(mode_part_str,mode_part);

  std::string ref_sym_str=props.get_optional_property("reflection_symmetry","-");
  reflection_symmetry.resize(0);
  if (ref_sym_str!="-")
  {
    std::stringstream ss(ref_sym_str);
    mbl_parse_int_list(ss, std::back_inserter(reflection_symmetry),
                       unsigned());
  }

  only_reflect=vul_string_to_bool(props.get_optional_property("only_reflect","false"));


  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

//: Use curves object to store the different parts
//  If mode_parts[i]==parts[j].name(),
//  then used[i] computed from parts[j].index()
//  If modes_parts[i]==all, then all elements used.
void get_pts_used_for_modes(const msm_curves& parts,
                        const std::vector<std::string>& mode_parts,
                        std::vector<std::vector<unsigned> >& pts_used)
{
  pts_used.resize(mode_parts.size());

  for (unsigned i=0;i<mode_parts.size();++i)
  {
    int p = parts.which_curve(mode_parts[i]);
    if (mode_parts[i]=="all" || p<0)
    {
      // Indicate that we should use all points
      pts_used[i].resize(0);
    }
    else
    {
      // Use indices from part p
      pts_used[i]=parts[p].index();
    }
  }
}


int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> out_path("-o","Path to which to save model (over-riding param file)");
  vul_arg<std::string> mode_var_path("-vp","Path for output of mode variances");
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

  if (out_path()!="")
    params.shape_model_path=out_path();

  // Load in curves if required
  msm_curves parts_for_modes;
  if (params.parts_for_modes_path!="")
  {
    if (!parts_for_modes.read_text_file(params.parts_for_modes_path))
    {
      std::cerr<<"Failed to read in parts from "
              <<params.parts_for_modes_path<<std::endl;
      return 2;
    }
  }

  // === Load in all the shapes ===
  unsigned n=params.points_names.size();
  std::vector<msm_points> shapes(n);
  msm_load_shapes(params.points_dir,params.points_names,shapes);

  if (params.reflection_symmetry.size()>0)
  {
    // Use reflections
    msm_points ref_points;
    for (unsigned i=0;i<n;++i)
    {
      msm_reflect_shape_along_x(shapes[i],params.reflection_symmetry,
                                ref_points,shapes[i].cog().x());
      if (params.only_reflect) shapes[i]=ref_points;
      else                     shapes.push_back(ref_points);
    }
  }

  msm_shape_model_builder builder;
  msm_shape_model shape_model;

  builder.set_aligner(*params.aligner);
  builder.set_ref_pose_source(params.ref_pose_source);
  builder.set_param_limiter(*params.limiter);
  builder.set_mode_choice(0,params.max_modes,params.var_prop);

  std::cout<<"Building shape model from "<<shapes.size()<<" examples."<<std::endl;

  if (params.mode_part.size()==0)
  {
    builder.build_model(shapes,shape_model);
  }
  else
  {
    std::vector<std::vector<unsigned> > pts_used;
    get_pts_used_for_modes(parts_for_modes,params.mode_part,pts_used);
    builder.build_model(shapes,pts_used,shape_model);
  }


  std::cout<<"Built model: "<<shape_model<<std::endl;

  if (vsl_quick_file_save(shape_model,params.shape_model_path))
  {
    std::cout<<"Saved shape model to "
            <<params.shape_model_path<<std::endl;
  }
  else
  {
    std::cerr<<"Failed to save to "<<params.shape_model_path<<'\n';
    return 3;
  }

  if (mode_var_path()!="")
  {
    std::ofstream ofs(mode_var_path().c_str());
    if (!ofs)
    {
      std::cerr<<"Failed to open "<<mode_var_path()<<" for output.\n";
      return 4;
    }
    for (unsigned i=0;i<shape_model.n_modes();++i)
      ofs<<shape_model.mode_var()[i]<<std::endl;
    ofs.close();
    std::cout<<"Saved mode variances to "<<mode_var_path()<<std::endl;
  }

  return 0;
}
