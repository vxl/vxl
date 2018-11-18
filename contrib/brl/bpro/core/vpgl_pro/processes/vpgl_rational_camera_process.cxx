// This is brl/bpro/core/vpgl_pro/processes/vpgl_rational_camera_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Operations related to vpgl_rational_camera
//
// \author Yi Dong
// \date April 22, 2014
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: Process to backproject an image point to a world point (wgs84 coords), using vpgl_backproject algo
//  Note that if initial gauss is not given, process will use camera offset as initial gauss
#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_2d.h>
#include <vpgl/vpgl_lvcs.h>
//: global variables and functions
namespace vpgl_rational_cam_img_to_global_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 3;
}
//: initialization
bool vpgl_rational_cam_img_to_global_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_rational_cam_img_to_global_process_globals;
  // process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[1] = "unsigned";                 // image point i
  input_types_[2] = "unsigned";                 // image point j
  input_types_[3] = "double";                   // initial gauss longitude (if given)
  input_types_[4] = "double";                   // initial gauss latitude  (if given)
  input_types_[5] = "double";                   // initial gauss elevation (also use as input plane elevation)
  input_types_[6] = "double";                   // plane elevation
  input_types_[7] = "double";                   // error tolerance
  // process takes 3 outputs
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";                  // world point lon
  output_types_[1] = "double";                  // world point lat
  output_types_[2] = "double";                  // world point elev
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
//: execute the process
bool vpgl_rational_cam_img_to_global_process(bprb_func_process& pro)
{
  using namespace vpgl_rational_cam_img_to_global_process_globals;

  // sanity check
  if (!pro.verify_inputs()){
    std::cerr << pro.name() << ": there should be " << n_inputs_ << " inputs" << std::endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);  // rational camera
  auto img_i = pro.get_input<unsigned>(i++);                              // image point i
  auto img_j = pro.get_input<unsigned>(i++);                              // image point j
  auto init_lon = pro.get_input<double>(i++);                                // initial gauss lon
  auto init_lat = pro.get_input<double>(i++);                                // initial gauss lat
  auto init_elev = pro.get_input<double>(i++);                                // initial gauss elev
  auto pl_elev = pro.get_input<double>(i++);                                // point plane height
  auto error_tol = pro.get_input<double>(i++);                                // error tolerance

  // get rational camera
  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cerr << pro.name() << ": the input camera is not a rational camera" << std::endl;
    return false;
  }

  std::cout << " rational camera type: " << rat_cam->type_name() << std::endl;

  // use camera offset if initial gauss is not given
  if (init_lon < 0.0) {
    // generate the initial from camera offset
    init_lon = rat_cam->offset(vpgl_rational_camera<double>::X_INDX);
    init_lat = rat_cam->offset(vpgl_rational_camera<double>::Y_INDX);
    init_elev = rat_cam->offset(vpgl_rational_camera<double>::Z_INDX);
  }
  // use init_elev as point plane height is pl_elev is not given
  if (pl_elev < 0.0)
    pl_elev = init_elev;

  std::cout << " initial gauss point: lon = " << init_lon << ", lat = " << init_lat << ", elev = " << init_elev << std::endl;
  std::cout << " elevation of the world point plane: " << pl_elev << std::endl;

  // start the back projection at given error tolerance
  vgl_point_2d<double> image_pt((double)img_i, (double)img_j);
  vgl_point_3d<double> world_pt(0.0, 0.0, 0.0);
  vgl_plane_3d<double> pl(0, 0, 1, -pl_elev);
  vgl_point_3d<double> initial_pt(init_lon, init_lat, init_elev);
  if (!vpgl_backproject::bproj_plane(rat_cam, image_pt, pl, initial_pt, world_pt, error_tol)) {
    std::cerr << pro.name() << ": back projection failed at given error initial gauss and error tolerance: " << error_tol << std::endl;
    return false;
  }
  double lon, lat, elev;
  lon = world_pt.x();  lat = world_pt.y();  elev = world_pt.z();
  // output
  i = 0;
  pro.set_output_val<double>(i++, lon);
  pro.set_output_val<double>(i++, lat);
  pro.set_output_val<double>(i++, elev);

  return true;
}

// take a lat, lon, elev position and compute the nadirness of the satellite image at that position. - measures in image space
// project (lat, lon, elev) to image plane
// project (lat, lon, elev+10) to image plane
// measure the difference in the image space
//  multiply the output of this process with the GSD of the image to get a nadirness value in meter units. that meter value would be comparable to other images' values if needed

bool vpgl_rational_cam_nadirness_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(4);
  input_types_[0] = "vpgl_camera_double_sptr";  // rational camera
  input_types_[1] = "double";                   // latitude
  input_types_[2] = "double";                   // longitude
  input_types_[3] = "double";                   // elevation (also use as input plane elevation)
  std::vector<std::string> output_types_(1);
  output_types_[0] = "double";                  // nadirness measure (in terms of pixels)
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}
bool vpgl_rational_cam_nadirness_process(bprb_func_process& pro)
{
  // sanity check
  if (!pro.verify_inputs()){
    std::cerr << pro.name() << ": there should be " << 4 << " inputs" << std::endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(i++);  // rational camera
  auto lat = pro.get_input<double>(i++);
  auto lon = pro.get_input<double>(i++);
  auto elev = pro.get_input<double>(i++);

  // get rational camera
  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cerr << pro.name() << ": the input camera is not a rational camera" << std::endl;
    return false;
  }

  std::cout << " rational camera type: " << rat_cam->type_name() << std::endl;
  double u1, v1, u2, v2;
  rat_cam->project(lon, lat, elev, u1, v1);
  rat_cam->project(lon, lat, elev+10, u2, v2);
  double dist = std::sqrt((u1-u2)*(u1-u2) + (v1-v2)*(v1-v2));
  pro.set_output_val<double>(0, dist);
  return true;
}

// take a rational camera and compute it rotation angle relative to north direciton
// it will return the rotation angle range from -Pi to Pi
bool vpgl_rational_cam_rotation_to_north_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(1);
  input_types_[0] = "vpgl_camera_double_sptr"; // rational camera
  std::vector<std::string> output_types_(1);
  output_types_[0] = "double";                  // rotation angle in degree
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vpgl_rational_cam_rotation_to_north_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!\n";
    return false;
  }
  // get the input
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(0);  // input rational camera
  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cerr << pro.name() << ": the input camera is not a rational camera!!\n";
    return false;
  }
  // get footprint of the rational camera
  double lon = rat_cam->offset(vpgl_rational_camera<double>::X_INDX);
  double lat = rat_cam->offset(vpgl_rational_camera<double>::Y_INDX);
  double z_off = rat_cam->offset(vpgl_rational_camera<double>::Z_INDX);
  double z_scale = rat_cam->scale(vpgl_rational_camera<double>::Z_INDX);
  double elev = z_off - z_scale;
  // project
  double u1, v1, u2, v2;
  rat_cam->project(lon, lat, elev, u1, v1);
  double north_lat = 0.01 + lat;
  rat_cam->project(lon, north_lat, elev, u2, v2);
  // compute the rotation angle that require to make image rotate to "north is up"
  vgl_vector_2d<double> v_ori_up(0, -v1);
  vgl_vector_2d<double> v_north(u2-u1, v2-v1);
  // output
  double out_ang = signed_angle(v_north, v_ori_up);
  pro.set_output_val<double>(0, out_ang);
  return true;
}

bool vpgl_rational_cam_rotation_to_up_vector_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types_(1);
  input_types_[0] = "vpgl_camera_double_sptr"; // rational camera
  std::vector<std::string> output_types_(2);
  output_types_[0] = "double"; // up direction in u
  output_types_[1] = "double"; // up direction in v
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vpgl_rational_cam_rotation_to_up_vector_process(bprb_func_process& pro)
{
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!\n";
    return false;
  }
  // get the input
  vpgl_camera_double_sptr cam_sptr = pro.get_input<vpgl_camera_double_sptr>(0);  // input rational camera
  auto* rat_cam = dynamic_cast<vpgl_rational_camera<double>*>(cam_sptr.as_pointer());
  if (!rat_cam) {
    std::cerr << pro.name() << ": the input camera is not a rational camera!!\n";
    return false;
  }
  // get footprint of the rational camera
  double lon = rat_cam->offset(vpgl_rational_camera<double>::X_INDX);
  double lat = rat_cam->offset(vpgl_rational_camera<double>::Y_INDX);
  double z_off = rat_cam->offset(vpgl_rational_camera<double>::Z_INDX);
  double z_scale = rat_cam->scale(vpgl_rational_camera<double>::Z_INDX);
  double elev = z_off - z_scale;
  // project
  double u1, v1, u2, v2;
  rat_cam->project(lon, lat, elev, u1, v1);
  rat_cam->project(lon, lat, elev+1, u2, v2);
  pro.set_output_val<double>(0, u2-u1);
  pro.set_output_val<double>(1, v2-v1);
  return true;
}
