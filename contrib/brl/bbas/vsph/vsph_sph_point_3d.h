#ifndef vsph_sph_point_3d_h_
#define vsph_sph_point_3d_h_
//:
// \file
#include <iostream>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: a point in the spherical coordinate system
// theta is elevation with zero at the North pole
// phi is azimuth with zero pointing East, positive heading North

class vsph_sph_point_3d
{
 public:
  //: Default constructor
  vsph_sph_point_3d() : radius_(1.0), theta_(0.0), phi_(0.0) {}

  vsph_sph_point_3d(double r, double theta, double phi) : radius_(r), theta_(theta), phi_(phi) {}

  ~vsph_sph_point_3d() = default;

  void set(double r, double theta, double phi) { radius_=r; theta_=theta; phi_=phi; }

  void print(std::ostream& os) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os);

  short version() const {return 1;}

  double radius_;
  double theta_;
  double phi_;
};

std::ostream& operator<<(std::ostream& os, vsph_sph_point_3d const& p);

void vsl_b_write(vsl_b_ostream& os, vsph_sph_point_3d& p);
void vsl_b_read(vsl_b_istream &is, vsph_sph_point_3d& p);


#endif
