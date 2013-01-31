#ifndef vsph_utils_h_
#define vsph_utils_h_
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include "vsph_sph_box_2d.h"
#include "vsph_sph_point_2d.h"
//: utility functions on the sphere
// theta is elevation with zero at the North Pole, 180 degress at the South Pole
// phi is azimuth with zero pointing East, positive heading North
class vsph_utils
{
 public:
  //find the smallest difference, (b-a), between azimuth angles
  //where the cut is at +-pi
  //counter-clockwise differences are positive
  //differences greater in magnitude than 180 reverse sign
  // that is, the sense of the smallest circular arc is used
  static double azimuth_diff(double phi_a, double phi_b, bool in_radians = true);
  //:the "distance" between two points on the unit sphere
  // currently the manhattan distance in elevation and azimuth
  // should be the arc of the great circle passing through the points -FIX!
  static double distance_on_usphere(vsph_sph_point_2d const& a,
				    vsph_sph_point_2d const& b);

  static bool a_eq_b(double phi_a, double phi_b, bool in_radians = true);
  static bool a_lt_b(double phi_a, double phi_b, bool in_radians = true);
  static vsph_sph_box_2d intersection(vsph_sph_box_2d const& b1,
                                      vsph_sph_box_2d const& b2);
 private:
  vsph_utils();
};

#endif
