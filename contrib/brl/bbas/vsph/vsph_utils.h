#ifndef vsph_utils_h_
#define vsph_utils_h_
//:
// \file
#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_perspective_camera.h>
#include "vsph_sph_box_2d.h"
#include "vsph_sph_point_2d.h"

//: utility functions on the sphere
// \a theta is elevation with zero at the North Pole, 180 degress at the South Pole
// \a phi is azimuth with zero pointing East, positive heading North
class vsph_utils
{
 public:
  //find the smallest difference, (b-a), between azimuth angles
  //where the cut is at +-pi
  //counter-clockwise differences are positive
  //differences greater in magnitude than 180 reverse sign
  // that is, the sense of the smallest circular arc is used
  static double azimuth_diff(double phi_a, double phi_b, bool in_radians = true);

  //find the sum a + b. handles the case if a and a+b are on opposite sides of the +-pi cut
  static double azimuth_sum(double phi_a, double phi_b, bool in_radians = true);

  //:find the angles half way between two angles on the circle, cut at +-pi
  // The result is ambiguous since two points on a circle define two arcs
  static void half_angle(double phi_a, double phi_b, double& ang_1,
                         double& ang_2,bool in_radians = true);

  //: The "distance" between two points on the unit sphere
  // Currently the Manhattan distance in elevation and azimuth
  // Should be the arc of the great circle passing through the points -FIX!
  static double distance_on_usphere(vsph_sph_point_2d const& a,
                                    vsph_sph_point_2d const& b);

  static bool a_eq_b(double phi_a, double phi_b, bool in_radians = true);
  static bool a_lt_b(double phi_a, double phi_b, bool in_radians = true);

  //: compute the ray passing through a given pixel.
  // the units of elevation and azimuth are set according to the choice,
  // {"radians", "degrees"}
  static void
    ray_spherical_coordinates(vpgl_perspective_camera<double> const& cam,
                              double u, double v,
                              double& elevation, double& azimuth,
                              const std::string& units = "radians");

  //: project an image polygon onto the surface of the unit sphere with origin at the camera center.
  // The edges of the spherical polygon are circular arcs but only the vertices
  // are defined in this function. Each vertex lies on the surface of the sphere.
  //
  // Note that the camera is needed only to set the focal length. The
  // camera rotation parameters only change the location of the region on
  // the sphere (i.e. translation on the surface of the sphere).
  //
  static vgl_polygon<double>
    project_poly_onto_unit_sphere(vpgl_perspective_camera<double> const& cam,
                                  vgl_polygon<double> const& image_poly,
                                  const std::string& units = "radians");

  // find the spherical bounding box for an image with camera
  static vsph_sph_box_2d box_from_camera(vpgl_perspective_camera<double> const& cam, const std::string& units);
    //read a set of unit sphere ray data (assumed in unsigned char format)
  static bool read_ray_index_data(const std::string& path, std::vector<unsigned char>& data);
  //total azimuth arc length from min_ph through c_ph to max_ph.
  static double arc_len(double min_ph, double max_ph, double c_ph);

  //is a azimuth value in the interval of a box
  static bool azimuth_in_interval(double phi, double a_phi_, double b_phi,
                                  double c_phi, bool in_radians);
  // a streamlined function for porting to parallel computation
  static double sph_inter_area(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2);
 private: // only static functions in this class: disallow instantiation
  vsph_utils() = delete;
};

#endif
