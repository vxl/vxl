//:
// \file
// \brief Tool to estimate how accurately a model can fit to sets of points
// \author Tim Cootes
// Given a model and a set of shapes, estimates the statistics of the errors on each point
// after fitting the model to each shape.
// [Later versions should allow leave-some-out tests]

//  *** Incomplete ***
//  Need to add option to
//  a) Load in different set of images
//  b) Test with different numbers of modes
//  c) Perform leave-some-out tests

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
#include <msm/msm_shape_instance.h>

#include <msm/msm_add_all_loaders.h>
#include <mbl/mbl_stats_1d.h>

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

ref0: 0 ref1: 1

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
  vcl_cout << "msm_build_shape_model -p param_file -t test_points_list.txt\n"
           << "Builds the shape model from the supplied data, tests on shapes in test_points_list.txt\n"
           << "If no test_points_list.txt provided, tests on training data."
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

  //: Ref. point indices used to define reference length.
  unsigned ref0,ref1;

  //: Directory containing images
  vcl_string image_dir;

  //: Directory containing points
  vcl_string points_dir;

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

  ref0=vul_string_atoi(props.get_optional_property("ref0","0"));
  ref1=vul_string_atoi(props.get_optional_property("ref1","0"));

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

struct msm_test_stats {
  mbl_stats_1d mean_d_stats;
  mbl_stats_1d rel_d_stats;
};

void calc_point_distances(const msm_points& points1, // labelled
                            const msm_points& points2,    // predicted
                            vnl_vector<double>& d)
{
  d.set_size(points1.size());
  d.fill(0.0);
  for (unsigned i=0;i<points1.size();++i)
    d[i]=(points1[i]-points2[i]).length();
}


void test_model(const msm_shape_model& shape_model, int n_modes,
                const vcl_vector<msm_points>& points,
                unsigned ref0, unsigned ref1,
                msm_test_stats& stats)
{
  msm_shape_instance sm_inst(shape_model);

  if (n_modes>=0)
  {
    // Arrange to use n_modes modes
    vnl_vector<double> b(n_modes,0.0);
    sm_inst.set_params(b);
  }

  vnl_vector<double> d;

  for (unsigned i=0;i<points.size();++i)
  {
    sm_inst.fit_to_points(points[i]);

    // Currently just compute overall distance
    // Eventually need to project into model frame to compute individual errors
    calc_point_distances(sm_inst.points(),points[i],d);
    stats.mean_d_stats.obs(d.mean());
    if (ref0!=ref1)
    {
      double ref_d = (points[i][ref0]-points[i][ref1]).length();
      stats.rel_d_stats.obs(100*d.mean()/ref_d);
    }
  }
}

int main(int argc, char** argv)
{
  vul_arg<vcl_string> param_path("-p","Parameter filename");
  vul_arg<vcl_string> test_list_path("-t","List of points files to test on");
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

  vcl_vector<msm_points> shapes(params.points_names.size());
  msm_load_shapes(params.points_dir,params.points_names,shapes);

  builder.build_model(shapes,shape_model);
  vcl_cout<<"Shape Model: "<<shape_model<<vcl_endl;

  // Test with differing numbers of modes
  for (unsigned nm=0;nm<=shape_model.n_modes();++nm)
  {
    msm_test_stats test_stats;

    test_model(shape_model,nm,shapes,params.ref0,params.ref1,test_stats);

    vcl_cout<<"=== "<<nm<<" Modes ==="<<vcl_endl;
    vcl_cout<<"Mean distance: "<<test_stats.mean_d_stats.mean()<<vcl_endl;
    if (params.ref0!=params.ref1)
      vcl_cout<<"Rel. distance (%): "<<test_stats.rel_d_stats.mean()<<vcl_endl;
  }

  return 0;
}
