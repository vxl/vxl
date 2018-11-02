//This is brl/bpro/core/bbas_pro/processes/bbas_camera_angles_process.cxx
//:
// \file
#include <vector>
#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bbas_camera_angles_process_cons(bprb_func_process& pro)
{
  //inputs
  // 0: camera
  // 1: x position of focus point
  // 2: y position of focus point
  // 3: z position of focus point
  std::vector<std::string> input_types_(4);
  input_types_[0] = "vpgl_camera_double_sptr";
  input_types_[1] = "float";
  input_types_[2] = "float";
  input_types_[3] = "float";
  if (!pro.set_input_types(input_types_))
    return false;

  //output
  // 0: camera elevation
  // 1: camera azimuth
  std::vector<std::string> output_types_(2);
  output_types_[0] = "float";
  output_types_[1] = "float";
  return pro.set_output_types(output_types_);
}

bool bbas_camera_angles_process(bprb_func_process& pro)
{
  if (pro.n_inputs()<4)
  {
    std::cout << pro.name() << " The number of inputs should be 4" << std::endl;
    return false;
  }

  //get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);

  double pt_x = pro.get_input<float>(1);
  double pt_y = pro.get_input<float>(2);
  double pt_z = pro.get_input<float>(3);

  // make sure camera is using a local coordinate frame
  auto *local_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr());
  if (!local_cam) {
    auto *rcam = dynamic_cast<vpgl_rational_camera<double>*>(camera.ptr());
    if (rcam) {
      // rational camera operates on geodetic coordinates, but we need a Euclidean space.
      double lat = pt_y;
      double lon = pt_x;
      double el = pt_z;
      vpgl_lvcs lvcs(lat, lon, el, vpgl_lvcs::wgs84, 0.0, 0.0, vpgl_lvcs::DEG, vpgl_lvcs::METERS);
      camera = new vpgl_local_rational_camera<double>(lvcs,*rcam);
      // should return 0,0,0 since point was used as origin
      lvcs.global_to_local(lon, lat, el, vpgl_lvcs::wgs84, pt_x, pt_y, pt_z);
    }
  }

  vgl_point_3d<double> focus_pt(pt_x, pt_y, pt_z);

  // project point into image
  double pt_u, pt_v;
  camera->project(pt_x,pt_y,pt_z, pt_u,pt_v);

  // backproject to plane above and below point
  constexpr double plane_dist = 10.0; // arbitrary distance above and below focus point to backproject to.
  const double z_low = pt_z - plane_dist;
  const double z_high = pt_z + plane_dist;
  vgl_point_2d<double> img_pt(pt_u,pt_v);
  vgl_point_3d<double> focus_pt_high(pt_x, pt_y, z_high);
  vgl_point_3d<double> focus_pt_low(pt_x, pt_y, z_low);
  vgl_plane_3d<double> plane_high(0.0, 0.0, 1.0, -z_high);
  vgl_plane_3d<double> plane_low(0.0, 0.0, 1.0, -z_low);
  vpgl_backproject::bproj_plane(camera.ptr(), img_pt, plane_high, focus_pt_high, focus_pt_high);
  vpgl_backproject::bproj_plane(camera.ptr(), img_pt, plane_low, focus_pt_low, focus_pt_low);

  // camera direction is vector from point_low to point_high (assuming camera is far above focus_pt with up ==  +z)
  vgl_vector_3d<double> cam_direction = focus_pt_high - focus_pt_low;
  normalize(cam_direction);

  // NOTE: assumes that local coordinate system has x = east and y = north
  double cam_az = std::atan2(cam_direction.x(), cam_direction.y());
  double cam_el = std::asin(cam_direction.z());

  //store azimuth and elevation in range 0,360
  pro.set_output_val<float>(0, float(vnl_math::angle_0_to_2pi(cam_az)*vnl_math::deg_per_rad));
  pro.set_output_val<float>(1, float(vnl_math::angle_0_to_2pi(cam_el)*vnl_math::deg_per_rad));

  return true;
}
