//:
// \file
// \brief Tool to reset a number of shape modes to 0 and save the updated point positions.
// \author Claudia Lindner
// Given a model and a set of shape points, selected shape modes are set to 0 and the
// shape points are updated (excluding the reset modes).

#include <iostream>
#include <iterator>
#include <limits>
#include <climits>
#include <mbl/mbl_read_props.h>
#include <mbl/mbl_exception.h>
#include <mbl/mbl_parse_int_list.h>
#include <mbl/mbl_parse_string_list.h>
#include <vul/vul_arg.h>
#include <vul/vul_string.h>
#include <vul/vul_file.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_quick_file.h>
#include <msm/msm_shape_instance.h>
#include <msm/msm_add_all_loaders.h>

/*
Parameter file format:
<START FILE>
//: File to read model from
shape_model_path: femur.msm

//: Shape model modes to be excluded (i.e. reset to 0)
modes_to_reset: { 5 9 13 14 15 }

//: Points to read in
points_dir: /home/points/
points: {
  points1.pts
  points2.pts
}

//: Updated points to be saved
out_points_dir: /home/points_new/
<END FILE>
*/

void print_usage()
{
  std::cout << "msm_reset_shape_modes -p param_file\n"
           << "Resets selected modes to 0 and saves updated model points."
           << std::endl;

  vul_arg_display_usage_and_exit();
}

//: Structure to hold parameters
struct tool_params
{
   //: File to read model from
  std::string shape_model_path;

  //: Shape model modes to be excluded (i.e. reset to 0)
  std::vector<unsigned> modes_to_reset;

  //: Directory containing points
  std::string points_dir;

  //: Directory containing points
  std::string out_points_dir;

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

  std::string modes_to_reset_str
       =props.get_required_property("modes_to_reset");
  modes_to_reset.resize(0);
  if (modes_to_reset_str!="")
  {
    std::stringstream ss(modes_to_reset_str);
    mbl_parse_int_list(ss, std::back_inserter(modes_to_reset),
                       unsigned());
  }

  points_dir=props.get_optional_property("points_dir","./");
  out_points_dir=props.get_optional_property("out_points_dir",points_dir+"/points_new/");

  mbl_parse_string_list(props.get_required_property("points"),points_names);

  try
  {
    mbl_read_props_look_for_unused_props(
      "::read_from_file", props, mbl_read_props_type());
  }
  catch (mbl_exception_unused_props& e)
  {
    throw (mbl_exception_parse_error(e.what()));
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

  // load shape model
  msm_shape_model shape_model;
  if (!vsl_quick_file_load(shape_model,params.shape_model_path))
  {
    std::cerr<<"Failed to load shape model from "
            <<params.shape_model_path<<'\n';
    return 2;
  }
  msm_shape_instance sm_inst(shape_model);
  const msm_aligner& aligner = shape_model.aligner();

  if (params.modes_to_reset.size() <= 0)
  {
    std::cerr<<"No shape model modes to be reset specified.\n"
            <<"Please choose values between 0 and "
            << sm_inst.params().size()<<" for modes_to_reset.\n";
    return 3;
  }

  // check for valid modes
  unsigned int min = std::numeric_limits<unsigned int>::max();
  unsigned int max = std::numeric_limits<unsigned int>::min();
  for (unsigned int k : params.modes_to_reset)
  {
    if (min > k) min = k;
    if (max < k) max = k;
  }
  if ((min <= 0) || (max > sm_inst.params().size()))
  {
    std::cerr<<"Invalid shape model mode index found in modes_to_reset.\n"
            <<"Please choose values between 1 and "
            << sm_inst.params().size()<<".\n";
    return 4;
  }

  // make sure not to overwrite points
  if (params.out_points_dir==params.points_dir)
  {
    std::cerr<<"out_points_dir==points_dir\n"
            <<"I'm not prepared to over-write the provided points.\n"
            <<"Create a new directory."<<std::endl;
    return 5;
  }

  // check that output directory exists
  if (!vul_file::is_directory(params.out_points_dir))
  {
    std::cout<<"Directory "<<params.out_points_dir
            <<" does not exist. Creating it."<<std::endl;
    if (!vul_file::make_directory_path(params.out_points_dir))
    {
      std::cerr<<"Unable to create it."<<std::endl;
      return 6;
    }
  }

  // cycle through point files
  unsigned n_point_files = params.points_names.size();
  for (unsigned ifile=0;ifile<n_point_files;++ifile)
  {
    msm_points points;
    std::string pts_path = params.points_dir+"/"+params.points_names[ifile];

    if (!points.read_text_file(pts_path))
    {
      std::cerr<<"Failed to load in points from "<<pts_path<<std::endl;
      return 7;
    }

    // fit shape model to original points
    sm_inst.fit_to_points(points);

    // transform points into reference frame
    msm_points ref_points;
    vnl_vector<double> inv_pose = aligner.inverse(sm_inst.pose());
    aligner.apply_transform(points,inv_pose,ref_points);

    // calculate residuals (on a point-by-point basis)
    vnl_vector<double> residual = ref_points.vector() - sm_inst.model_points().vector();

    // zero selected shape model modes
    vnl_vector<double> b = sm_inst.params();
    for (unsigned int k : params.modes_to_reset)
    { b[k-1]=0; }
    sm_inst.set_params(b);

    // get modified shape and put residuals back in
    msm_points new_ref_points = sm_inst.model_points();
    new_ref_points.vector() += residual;

    // transform points back into world frame
    msm_points new_points;
    aligner.apply_transform(new_ref_points,sm_inst.pose(),new_points);

    // save updated points
    std::string pts_new_path = params.out_points_dir+"/"+params.points_names[ifile];
    if (!new_points.write_text_file(pts_new_path))
    {
        std::cerr<<"Cannot write updated points file to: "<<params.out_points_dir<<std::endl;
        return 8;
    }
  }

  return 0;
}
