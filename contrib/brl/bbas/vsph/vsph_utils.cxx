#include "vsph_utils.h"
#include <vnl/vnl_math.h>

// compute b-a on the circle
double vsph_utils::azimuth_diff(double phi_a, double phi_b,
				bool in_radians){
  double a = phi_a, b = phi_b, diff = 0.0, pi = vnl_math::pi,
    pi2 = vnl_math::pi_over_2, two_pi = vnl_math::twopi;
  if(!in_radians){
    a /= vnl_math::deg_per_rad;
    b /= vnl_math::deg_per_rad;
  }
  diff = b -a;
  if(diff > pi)
    diff -= two_pi;
  if(diff < -pi)
    diff += two_pi;
  if(in_radians)
    return diff;
  return diff*vnl_math::deg_per_rad;
}
bool vsph_utils::a_eq_b(double phi_a, double phi_b, bool in_radians){
  double a = phi_a, b = phi_b, pi = vnl_math::pi;
  if(!in_radians){
    a /= vnl_math::deg_per_rad;
    b /= vnl_math::deg_per_rad;
  }
  if(((a == -pi)&&(b == pi))||((a == pi)&&(b == -pi))) return true;
  return (a == b);
}
bool vsph_utils::a_lt_b(double phi_a, double phi_b, bool in_radians){
  double diff = vsph_utils::azimuth_diff(phi_a, phi_b, in_radians);
  // a < b if (b-a) > 0
  return diff > 0;
}
  
vsph_sph_box_2d vsph_utils::intersection(vsph_sph_box_2d const& b1,
					 vsph_sph_box_2d const& b2){
  vsph_sph_box_2d rbox;

  double phi_min = 
    vsph_utils::a_lt_b(b1.min_phi(),b2.min_phi()) ? b2.min_phi() : b1.min_phi();

  double phi_max = 
    vsph_utils::a_lt_b(b1.max_phi(),b2.max_phi()) ? b1.max_phi() : b2.max_phi();

  double theta_min = 
    b1.min_theta() < b2.min_theta() ? b2.min_theta() : b1.min_theta();

  double theta_max = 
    b1.max_theta() < b2.max_theta() ? b1.max_theta() : b2.max_theta();
  
  //arbitrarily set units according to b1
  rbox.add(theta_min, phi_min, b1.in_radians());
  rbox.add(theta_max, phi_max, b1.in_radians());

  return rbox;
}
