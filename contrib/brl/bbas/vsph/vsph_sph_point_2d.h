#ifndef vsph_sph_point_2d_h_
#define vsph_sph_point_2d_h_
//:
// \file
#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: a point in the spherical coordinate system on the surface of a unit sphere
// \p theta is elevation with zero at the North Pole, 180 degress at the South Pole
// \p phi is azimuth with zero pointing East, positive heading North
class vsph_sph_point_2d
{
 public:
  //: Default constructor
  vsph_sph_point_2d() : theta_(0.0), phi_(0.0) {}

  vsph_sph_point_2d(double theta, double phi, bool in_radians = true);

  ~vsph_sph_point_2d() = default;

  void set(double theta, double phi, bool in_radians = true)
  { in_radians_ = in_radians; theta_=theta; phi_=phi; }

  bool operator==(const vsph_sph_point_2d &other) const;


  void print(std::ostream& os) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os) const;

  short version() const {return 1;}
  bool in_radians_;
  double theta_;
  double phi_;
};

void vsl_b_read(vsl_b_istream& is, vsph_sph_point_2d& sp);

void vsl_b_write(vsl_b_ostream& os, vsph_sph_point_2d const& sp);

void vsl_print_summary(std::ostream& os, vsph_sph_point_2d const& sp);

std::ostream& operator<<(std::ostream& os, vsph_sph_point_2d const& sp);

#endif
