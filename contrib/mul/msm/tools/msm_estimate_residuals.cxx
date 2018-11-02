//:
// \file
// \brief Tool to estimate how accurately a model can fit to sets of points
// \author Tim Cootes
// Given a model and a set of shapes, estimates the statistics of the errors on each point
// after fitting the model to each shape.
// Can either perform leave-some-out experiments on training set, or apply the model
// to a different set.

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

// Optional indicies of points used to define a reference length
ref0: 0 ref1: 1

// Number of chunks in n-fold cross validation
n_chunks: 10

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
  std::cout << "msm_build_shape_model -p param_file -t test_points_list.txt\n"
           << "Builds the shape model from the supplied data, tests on shapes in test_points_list.txt\n"
           << "If no test_points_list.txt provided, performs leave-some-out tests on training data."
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
  //: Aligner for shape model
  std::unique_ptr<msm_aligner> aligner;

  //: Object to apply limits to parameters
  std::unique_ptr<msm_param_limiter> limiter;

  //: Maximum number of shape modes
  unsigned max_modes;

  //: Proportion of shape variation to explain
  double var_prop;

  //: Ref. point indices used to define reference length.
  unsigned ref0,ref1;

  //: Number of chunks in n-fold cross validation
  unsigned n_chunks;

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

  max_modes=vul_string_atoi(props.get_optional_property("max_modes","99"));
  var_prop=vul_string_atof(props.get_optional_property("var_prop","0.95"));
  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");

  ref0=vul_string_atoi(props.get_optional_property("ref0","0"));
  ref1=vul_string_atoi(props.get_optional_property("ref1","0"));
  n_chunks=vul_string_atoi(props.get_optional_property("n_chunks","10"));

  {
    std::string aligner_str
       = props.get_required_property("aligner");
    std::stringstream ss(aligner_str);
    aligner = msm_aligner::create_from_stream(ss);
  }

  {
    std::string limiter_str
       = props.get_optional_property("param_limiter",
                                     "msm_ellipsoid_limiter { accept_prop: 0.98 }");
    std::stringstream ss(limiter_str);
    limiter = msm_param_limiter::create_from_stream(ss);
  }

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}

//: Structure to hold parameters of image list file
struct image_list_params
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
void image_list_params::read_from_file(const std::string& path)
{
  std::ifstream ifs(path.c_str());
  if (!ifs)
  {
    std::string error_msg = "Failed to open file: "+path;
    throw (mbl_exception_parse_error(error_msg));
  }

  mbl_read_props_type props = mbl_read_props_ws(ifs);

  image_dir=props.get_optional_property("image_dir","./");
  points_dir=props.get_optional_property("points_dir","./");

  mbl_parse_colon_pairs_list(props.get_required_property("images"),
                             points_names,image_names);

  // Don't look for unused props so can use a single common parameter file.
}



struct msm_test_stats {
  //: Stats of mean distance in world frame
  mbl_stats_1d world_d_stats;
  //: Stats of mean distance relative to a reference length (%)
  mbl_stats_1d rel_d_stats;
  //: Stats of mean distance in model reference frame
  mbl_stats_1d ref_d_stats;

  //: Stats of residual x in reference frame
  mbl_stats_1d ref_x_stats;
  //: Stats of residual y in reference frame
  mbl_stats_1d ref_y_stats;
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
                const std::vector<msm_points>& points,
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

  vnl_vector<double> d,inv_pose;
  const msm_aligner& aligner = shape_model.aligner();
  msm_points points_in_ref, dpoints;

  for (const auto & point : points)
  {
    sm_inst.fit_to_points(point);

    // Currently just compute overall distance
    // Eventually need to project into model frame to compute individual errors
    calc_point_distances(sm_inst.points(),point,d);
    stats.world_d_stats.obs(d.mean());
    if (ref0!=ref1)
    {
      double ref_d = (point[ref0]-point[ref1]).length();
      stats.rel_d_stats.obs(100*d.mean()/ref_d);
    }

    // Evaluate in the reference frame
    inv_pose=aligner.inverse(sm_inst.pose());
    aligner.apply_transform(point,inv_pose,points_in_ref);
    calc_point_distances(sm_inst.model_points(),points_in_ref,d);
    stats.ref_d_stats.obs(d.mean());

    dpoints.vector()=points_in_ref.vector()-sm_inst.model_points().vector();
    for (unsigned j=0;j<dpoints.size();++j)
    {
      stats.ref_x_stats.obs(dpoints[j].x());
      stats.ref_y_stats.obs(dpoints[j].y());
    }
  }
}


// Perform leave-some-out experiments, chopping data into n_chunks chunks
void leave_some_out_tests(msm_shape_model_builder& builder,
                          const std::vector<msm_points>& points,
                          unsigned ref0, unsigned ref1,
                          unsigned n_chunks,
                          std::vector<msm_test_stats>& test_stats)
{
  // Arrange to miss out consecutive examples.
  double chunk_size=double(points.size())/n_chunks;
  if (chunk_size<1) return;

  for (unsigned ic=0;ic<n_chunks;++ic)
  {
    std::vector<msm_points> trn_set,test_set;
    for (unsigned i=0;i<points.size();++i)
    {
      if (unsigned(i/chunk_size)==ic) test_set.push_back(points[i]);
      else                            trn_set.push_back(points[i]);
    }
    msm_shape_model shape_model;
    builder.build_model(trn_set,shape_model);

    for (unsigned nm=0;nm<=shape_model.n_modes();++nm)
    {
      test_model(shape_model,nm,test_set,ref0,ref1,test_stats[nm]);
    }
  }
}

int main(int argc, char** argv)
{
  vul_arg<std::string> param_path("-p","Parameter filename");
  vul_arg<std::string> test_list_path("-t","List of points files to test on");
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

  image_list_params image_list;
  if (test_list_path()!="")
  {
    try
    {
      image_list.read_from_file(test_list_path());
    }
    catch (mbl_exception_parse_error& e)
    {
      std::cerr<<"Error: "<<e.what()<<'\n';
      return 1;
    }
  }

  msm_shape_model_builder builder;

  builder.set_aligner(*params.aligner);
  builder.set_param_limiter(*params.limiter);
  builder.set_mode_choice(0,params.max_modes,params.var_prop);

  std::vector<msm_points> shapes(params.points_names.size());
  msm_load_shapes(params.points_dir,params.points_names,shapes);

  std::vector<msm_test_stats> test_stats(params.max_modes+1);

  if (test_list_path()!="")
  {
    std::cout<<"Testing on "<<image_list.points_names.size()<<" examples from "<<test_list_path()<<std::endl;
    msm_shape_model shape_model;
    builder.build_model(shapes,shape_model);
    std::cout<<"Shape Model: "<<shape_model<<std::endl;

    std::vector<msm_points> test_shapes(image_list.points_names.size());
    msm_load_shapes(image_list.points_dir,image_list.points_names,test_shapes);
    // Test with differing numbers of modes
    for (unsigned nm=0;nm<=shape_model.n_modes();++nm)
    {
      test_model(shape_model,nm,test_shapes,params.ref0,params.ref1,test_stats[nm]);
    }
  }
  else
  {
    std::cout<<"Performing "<<params.n_chunks<<"-fold cross validation on "<<shapes.size()<<" examples from training set."<<std::endl;
    leave_some_out_tests(builder,shapes,params.ref0,params.ref1,params.n_chunks,test_stats);
  }

  std::cout<<"NModes WorldMean   RefMean ";
  if (params.ref0!=params.ref1) std::cout<<"     RelMean(%)";
  std::cout<<"RefXSD RefYSD ";
  std::cout<<std::endl;
  for (unsigned nm=0;nm<test_stats.size();++nm)
  {
    if (test_stats[nm].world_d_stats.nObs()==0) continue;
    std::cout<<nm<<"      "<<test_stats[nm].world_d_stats.mean();
    std::cout<<"    "<<test_stats[nm].ref_d_stats.mean();
    if (params.ref0!=params.ref1)
      std::cout<<"    "<<test_stats[nm].rel_d_stats.mean();
    std::cout<<"    "<<test_stats[nm].ref_x_stats.sd();
    std::cout<<"    "<<test_stats[nm].ref_y_stats.sd();
    std::cout<<std::endl;
  }

  return 0;
}
