//This is brl/bbas/bpgl/bpgl_camera_utils.h
#ifndef bpgl_camera_utils_h_
#define bpgl_camera_utils_h_
//:
// \file
// \brief A class for various camera computations
//
// \author J. L. Mundy
// \date July 25, 2012
// \verbatim
//  Modifications
// \endverbatim


#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_line_2d.h>
#include <vpgl/vpgl_perspective_camera.h>
class bpgl_camera_utils
{
 public:
  //Calibrate a camera from the horizon line and a specification of
  //focal length in units of the focal plane pixel dimensions and
  //camera height off the ground plane in world units.
  //Assumes that the sensor pixel has a square aspect ratio.
  //a, b and c are the line coefficients of the horizon line
  // in image coordinates
  static vpgl_perspective_camera<double>
    camera_from_horizon(double focal_length,
                        double principal_pt_u,
                        double principal_pt_v,
                        double cam_height,
                        double a, double b, double c);

  // assumes that principal point is in the center of the image
  // the camera origin is at Cartesian (0,0, altitude)
  // see https://developers.google.com/kml/documentation/cameras
  // for definition of the kml camera parameters
  static vpgl_perspective_camera<double>
    camera_from_kml(double ni, double nj, double right_fov, double top_fov,
                    double altitude, double heading,
                    double tilt, double roll);

  // return a vector of cameras saved in the specified directory
  static std::vector<vpgl_perspective_camera<double>* > cameras_from_directory(const std::string& dir);

  // the horizon line for a given camera
  static vgl_line_2d<double>
    horizon(vpgl_perspective_camera<double> const& cam);

  static std::string get_string(double ni, double nj, double right_f, double top_f, double alt, double head, double tilt, double roll);

  static void camera_parameters(vpgl_perspective_camera<double> const& cam,
                                double& rodrigues_x, double& rodrigues_y,
                                double& rodriques_z, double& tx, double& ty,
                                double& tz, double& focal_length_u,
                                double& focal_length_v, double& principal_pt_u,
                                double& principal_pt_v);

  static bool camera_from_parameters(double rodrigues_x, double rodrigues_y,
                                     double rodriques_z, double tx, double ty,
                                     double tz, double focal_length_u,
                                     double focal_length_v,
                                     double principal_pt_u,
                                     double principal_pt_v,
                                     vpgl_perspective_camera<double>& cam);
};
#endif //bpgl_camera_utils_h_
