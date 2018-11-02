// This is brl/bpro/core/vpgl_pro/processes/vpgl_persp_cam_from_photo_overlay_process.cxx
#include <iostream>
#include <cmath>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing a perspective camera using Google Earth photo overlay parameters
// Viz. Tilt, Roll, Lat, Lon, altitude, Horizontal and Vertical Field of View
//
// \author Ozge C. Ozcanli
// \date June 06, 2012

#include <vpgl/vpgl_lvcs.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_camera_from_box.h>

namespace vpgl_persp_cam_from_photo_overlay_process_globals
{
  constexpr unsigned n_inputs_ = 11;
  constexpr unsigned n_outputs_ = 1;
}

bool vpgl_persp_cam_from_photo_overlay_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_persp_cam_from_photo_overlay_process_globals;

  //process takes 11 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vpgl_lvcs_sptr";
  input_types_[1] = "float"; // heading
  input_types_[2] = "float"; // tilt
  input_types_[3] = "float"; // roll
  input_types_[4] = "float"; // cam center latitude
  input_types_[5] = "float"; // cam center longitude
  input_types_[6] = "float"; // cam center altitude (above sea level)
  input_types_[7] = "float"; // horizontal field of view
  input_types_[8] = "float"; // vertical field of view
  input_types_[9] = "unsigned"; // ni
  input_types_[10] = "unsigned"; // nj

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";// longitude

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool vpgl_persp_cam_from_photo_overlay_process(bprb_func_process& pro)
{
  using namespace vpgl_persp_cam_from_photo_overlay_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  double deg_to_rad = vnl_math::pi/180.0;
  //get the inputs
  vpgl_lvcs_sptr lvcs = pro.get_input<vpgl_lvcs_sptr>(0);
  auto heading = pro.get_input<float>(1); // TODO - unused!
  auto tilt = pro.get_input<float>(2); // TODO - unused!
  auto roll = pro.get_input<float>(3); // TODO - unused!
  auto lat = pro.get_input<float>(4);
  auto lon = pro.get_input<float>(5);
  auto alt = pro.get_input<float>(6);
  double fov_hor = pro.get_input<float>(7)*deg_to_rad; // TODO - unused!
  double fov_ver = pro.get_input<float>(8)*deg_to_rad; // TODO - unused!
  auto ni = pro.get_input<unsigned>(9);
  auto nj = pro.get_input<unsigned>(10);

  double x,y,z;
  lvcs->global_to_local(lon, lat, alt, lvcs->get_cs_name(), x,y,z);
  vgl_homg_point_3d<double> camera_center(x,y,z);
  std::cout << "camera center in local coords: " << camera_center << std::endl;
#if 0
  vgl_point_3d<double> cam_cent(x,y,z);
  //vgl_point_3d<double> look_at(x,y,0);
  lvcs->global_to_local(lon, lat+1, alt, lvcs->get_cs_name(), x,y,z);
  vgl_point_3d<double> cam_cent_up(x,y,z);
  vgl_vector_3d<double> up_vector = cam_cent_up-cam_cent; // y axis of google

  // now rotate up_vector by heading around Z axis
  // heading is a rotation about Z axis
  vnl_quaternion<double> q_h(0.0, 0.0, -heading*deg_to_rad);
  vgl_rotation_3d<double> R_h(q_h);
  up_vector = R_h*up_vector;   // this is up vector of camera

  // find x axis wrt to up, cross product of -up vector and z
  vgl_vector_3d<double> z_vector(0.0, 0.0, 1.0);
  vgl_vector_3d<double> x_vector = cross_product(-up_vector,z_vector);
  normalize(x_vector);

  // now rotate up_vector by tilt around x vector
  vnl_vector_fixed<double, 3> x_vector_fixed(x_vector.x(), x_vector.y(), x_vector.z());
  vnl_quaternion<double> q_t(x_vector_fixed, tilt*deg_to_rad);
  vgl_rotation_3d<double> R_t(q_t);
  vgl_vector_3d<double> look_vector = R_t*up_vector;

  // now find look at point
  vgl_point_3d<double> look_at = cam_cent + look_vector;

  //horizontal field of view = 2 atan(0.5 width / focallength)
  double focal_length = ni/(2.0 * std::tan(fov_hor/2.0f));
  double focal_length2 = nj/(2.0 * std::tan(fov_ver/2.0f));
  std::cout << "focal length: " << focal_length << " focal_length 2: " << focal_length2 << std::endl;
#endif
  double focal_length = 1500;
  vnl_double_3x3 M;
  M[0][0] = focal_length; M[0][1] = 0; M[0][2] = ni/2;
  M[1][0] = 0; M[1][1] = focal_length; M[1][2] = nj/2;
  M[2][0] = 0; M[2][1] = 0; M[2][2] = 1;
  vpgl_calibration_matrix<double> K(M);
  std::cout << "initial K:\n" << M << std::endl;
#if 0
  vgl_rotation_3d<double> I; // no rotation initially
  vpgl_perspective_camera<double> camera(K, camera_center,I);
  camera.look_at(vgl_homg_point_3d<double>(look_at), up_vector);
#endif
  vgl_point_3d<double> scene_min(0,0,0);vgl_point_3d<double> scene_max(2400,1400,200);
  vgl_box_3d<double> box(scene_min, scene_max);
  vpgl_perspective_camera<double> camera =
    bpgl_camera_from_box::persp_camera_from_box(box, camera_center, ni, nj);

  std::cout << "final cam:\n" << camera << std::endl;

  //set the output
  pro.set_output_val<vpgl_camera_double_sptr>(0, new vpgl_perspective_camera<double>(camera));
  return true;
}
