// This is brl/bseg/boxm2/view/pro/processes/boxm2_view_init_trajectory_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for initializing a boxm2_trajectory
//
// \author Andrew Miller
// \date Mar 10, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/view/boxm2_trajectory.h>
#include <vil/vil_image_view.h>

//brdb stuff
#include <brdb/brdb_value.h>

namespace boxm2_view_init_trajectory_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm2_view_init_trajectory_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_view_init_trajectory_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr"; //Scene
  input_types_[1] = "double"; //incline 0
  input_types_[2] = "double"; //incline 1
  input_types_[3] = "double"; //radius
  input_types_[4] = "unsigned"; // ni image width
  input_types_[5] = "unsigned"; // nj image height

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_trajectory_sptr";     //an initialized trajectory object

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_view_init_trajectory_process(bprb_func_process& pro)
{
  using namespace boxm2_view_init_trajectory_process_globals;
  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned argIdx = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  double           incline0 = pro.get_input<double>(argIdx++);
  double           incline1 = pro.get_input<double>(argIdx++);
  double           radius = pro.get_input<double>(argIdx++);
  unsigned         ni = pro.get_input<unsigned>(argIdx++);
  unsigned         nj = pro.get_input<unsigned>(argIdx++);

  //new trjaectory
  boxm2_trajectory_sptr traj = new boxm2_trajectory(incline0, incline1, radius, scene->bounding_box(), ni, nj);

  // store trajectory pointer
  argIdx = 0;
  pro.set_output_val<boxm2_trajectory_sptr>(argIdx++, traj);

  return true;
}


namespace boxm2_view_init_regular_trajectory_process_globals
{
  constexpr unsigned n_inputs_ = 15;
  constexpr unsigned n_outputs_ = 1;
}

//: initialize with the parametrization in a photo overlay given by a kml file
bool boxm2_view_init_regular_trajectory_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_view_init_regular_trajectory_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "unsigned";// ni
  input_types[1] = "unsigned";// nj
  input_types[2] = "double";  // right field of view
  input_types[3] = "double";  // top field of view
  input_types[4] = "double";  // altitude  -- this is altitude from ground level, e.g. on average 1.6 if camera is taken by a standing person
  input_types[5] = "double";  // heading -- all the angles are in degrees
  input_types[6] = "double";  // tilt
  input_types[7] = "double";  // roll

  input_types[8] = "double"; // x_start - in local coords, if lvcs is not rotated (theta = 0) then +x axis points towards East
  input_types[9] = "double"; // y_start - in local coords, if lvcs is not rotated (theta = 0) then +y axis points towards North
  input_types[10] = "double"; // x_end
  input_types[11] = "double"; // y_end
  input_types[12] = "double"; // x_increment
  input_types[13] = "double"; // y_increment
  input_types[14] = "double"; // heading_increment

  // process has 1 output:
  std::vector<std::string>  output_types(n_outputs_);
  output_types[0] = "boxm2_trajectory_sptr";     //an initialized trajectory object

  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool boxm2_view_init_regular_trajectory_process(bprb_func_process& pro)
{
  using namespace boxm2_view_init_regular_trajectory_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  double right_fov = pro.get_input<double>(i++);
  double top_fov = pro.get_input<double>(i++);
  double alt = pro.get_input<double>(i++);
  double heading = pro.get_input<double>(i++);
  double tilt = pro.get_input<double>(i++);
  double roll = pro.get_input<double>(i++);

  double x_start = pro.get_input<double>(i++);
  double y_start = pro.get_input<double>(i++);
  double x_end = pro.get_input<double>(i++);
  double y_end = pro.get_input<double>(i++);
  double x_increment = pro.get_input<double>(i++);
  double y_increment = pro.get_input<double>(i++);
  double heading_increment = pro.get_input<double>(i++);

  // new trjaectory
  boxm2_trajectory_sptr traj = new boxm2_trajectory(ni, nj, right_fov, top_fov, alt, heading, tilt, roll, x_start, y_start, x_end, y_end, x_increment, y_increment, heading_increment);

  // store trajectory pointer
  pro.set_output_val<boxm2_trajectory_sptr>(0, traj);
  return true;
}

namespace boxm2_view_init_height_map_trajectory_process_globals
{
  constexpr unsigned n_inputs_ = 18;
  constexpr unsigned n_outputs_ = 1;
}

//: initialize with the parametrization in a photo overlay given by a kml file
//  use the height map to place the camera at regular intervals in the scene
bool boxm2_view_init_height_map_trajectory_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_view_init_height_map_trajectory_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "boxm2_scene_sptr";
  input_types[1] = "vil_image_view_base_sptr";  // x image -- in local coords of the scene - if lvcs is not rotated (theta = 0) then +x axis points towards East
  input_types[2] = "vil_image_view_base_sptr";  // y image -- in local coords of the scene - if lvcs is not rotated (theta = 0) then +y axis points towards North
  input_types[3] = "vil_image_view_base_sptr";  // z image -- height map in local coords of the scene

  input_types[4] = "unsigned";// ni
  input_types[5] = "unsigned";// nj
  input_types[6] = "double";  // right field of view
  input_types[7] = "double";  // top field of view
  input_types[8] = "double";  // altitude  -- this is altitude from ground level, e.g. on average 1.6 if camera is taken by a standing person
  input_types[9] = "double";  // tilt  -- all the angles are in degrees
  input_types[10] = "double";  // roll

  input_types[11] = "unsigned"; // margin - in the height image, this many pixels will be skipped in the margins
  input_types[12] = "unsigned"; // i_start
  input_types[13] = "unsigned"; // j_start
  input_types[14] = "unsigned"; // i_increment - in the height image, this many pixles will be advanced to place the cameras, skips in x direction in local coords
  input_types[15] = "unsigned"; // j_increment - in the height image, this many pixles will be advanced to place the cameras, skips in y direction in local coords
  input_types[16] = "double"; // heading_start
  input_types[17] = "double"; // heading_increment

  // process has 1 output:
  std::vector<std::string>  output_types(n_outputs_);
  output_types[0] = "boxm2_trajectory_sptr";     //an initialized trajectory object

  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

bool boxm2_view_init_height_map_trajectory_process(bprb_func_process& pro)
{
  using namespace boxm2_view_init_height_map_trajectory_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  vil_image_view_base_sptr x_imgs = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr y_imgs = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr z_imgs = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<float> x_img(x_imgs);
  vil_image_view<float> y_img(y_imgs);
  vil_image_view<float> z_img(z_imgs);

  if (x_img.ni() != y_img.ni() || x_img.ni() != z_img.ni() ||
      x_img.nj() != y_img.nj() || x_img.nj() != z_img.nj() )
      return false;

  unsigned ni = pro.get_input<unsigned>(i++);
  unsigned nj = pro.get_input<unsigned>(i++);
  double right_fov = pro.get_input<double>(i++);
  double top_fov = pro.get_input<double>(i++);
  double alt = pro.get_input<double>(i++);
  double tilt = pro.get_input<double>(i++);
  double roll = pro.get_input<double>(i++);

  unsigned margin = pro.get_input<unsigned>(i++);
  unsigned i_start = pro.get_input<unsigned>(i++);
  unsigned j_start = pro.get_input<unsigned>(i++);
  unsigned i_inc = pro.get_input<unsigned>(i++);
  unsigned j_inc = pro.get_input<unsigned>(i++);
  double heading_start = pro.get_input<double>(i++);
  double heading_increment = pro.get_input<double>(i++);

  // new trjaectory
  boxm2_trajectory_sptr traj = new boxm2_trajectory(scene->bounding_box(), x_img, y_img, z_img, ni, nj, right_fov, top_fov, alt, tilt, roll, margin, i_start, j_start, i_inc, j_inc, heading_start, heading_increment);

  // store trajectory pointer
  pro.set_output_val<boxm2_trajectory_sptr>(0, traj);
  return true;
}
