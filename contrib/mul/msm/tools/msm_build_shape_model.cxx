//:
// \file
// \brief Tool to build a shape model from data in files.
// \author Tim Cootes

#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_colon_pairs_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vsl/vsl_quick_file.h>

#include <msm/msm_shape_model_builder.h>

#include <msm/msm_add_all_loaders.h>

/*
Parameter file format:
<START FILE>
//: Aligner for shape model
aligner: msm_similarity_aligner

//: Object to apply limits to parameters
param_limiter: msm_ellipsoid_limiter { accept_prop: 0.98 }

// Maximum number of shape modes
max_modes: 99

// Proportion of shape variation to explain
var_prop: 0.95

//: File to save model to
shape_model_path: shape_model.bfs

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
  vcl_cout << "msm_build_shape_model -p param_file\n"
           << "Builds the shape model from the supplied data.\n"
           << vcl_endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Aligner for shape model
  vcl_auto_ptr<msm_aligner> aligner;

  //: Object to apply limits to parameters
  vcl_auto_ptr<msm_param_limiter> limiter;

  //: Maximum number of shape modes
  unsigned max_modes;

  //: Proportion of shape variation to explain
  double var_prop;

  //: Directory containing images
  vcl_string image_dir;

  //: Directory containing points
  vcl_string points_dir;

  //: File to save shape model to
  vcl_string shape_model_path;

  //: List of image names
  vcl_vector<vcl_string> image_names;

  //: List of points file names
  vcl_vector<vcl_string> points_names;

  //: Parse named text file to read in data
  //  Throws a mbl_exception_parse_error if fails
  void read_from_file(const vcl_string& path);
};

//: Parse named text file to read in data
//  Throws a mbl_exception_parse_error if fails
void tool_params::read_from_file(const vcl_string& path)
{
  vcl_ifstream ifs(path.c_str());
  if (!ifs)
  {
    vcl_string error_msg = "Failed to open file: "+path;
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
    vcl_string aligner_str
       = props.get_required_property("aligner");
    vcl_stringstream ss(aligner_str);
    aligner = msm_aligner::create_from_stream(ss);
  }

  {
    vcl_string limiter_str
       = props.get_optional_property("param_limiter",
                                     "msm_ellipsoid_limiter { accept_prop: 0.98 }");
    vcl_stringstream ss(limiter_str);
    limiter = msm_param_limiter::create_from_stream(ss);
  }

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> param_path("-p","Parameter filename");
  vul_arg<vcl_string> mode_var_path("-vp","Path for output of mode variances");
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
    vcl_cerr<<"Error: "<<e.what()<<'\n';
    return 1;
  }

  msm_shape_model_builder builder;
  msm_shape_model shape_model;

  builder.set_aligner(*params.aligner);
  builder.set_param_limiter(*params.limiter);
  builder.set_mode_choice(0,params.max_modes,params.var_prop);
  builder.build_from_files(params.points_dir,
                           params.points_names,
                           shape_model);

  vcl_cout<<"Built model: "<<shape_model<<vcl_endl;

  if (vsl_quick_file_save(shape_model,params.shape_model_path))
  {
    vcl_cout<<"Saved shape model to "
            <<params.shape_model_path<<vcl_endl;
  }
  else
  {
    vcl_cerr<<"Failed to save to "<<params.shape_model_path<<'\n';
    return 3;
  }

  if (mode_var_path()!="")
  {
    vcl_ofstream ofs(mode_var_path().c_str());
    if (!ofs)
    {
      vcl_cerr<<"Failed to open "<<mode_var_path()<<" for output.\n";
      return 4;
    }
    for (unsigned i=0;i<shape_model.n_modes();++i)
      ofs<<shape_model.mode_var()[i]<<vcl_endl;
    ofs.close();
    vcl_cout<<"Saved mode variances to "<<mode_var_path()<<vcl_endl;
  }

  return 0;
}
