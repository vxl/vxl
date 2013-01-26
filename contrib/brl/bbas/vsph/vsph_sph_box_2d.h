#ifndef vsph_sph_box_2d_h_
#define vsph_sph_box_2d_h_
//:
// \file
#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vsph/vsph_sph_point_2d.h>
//: an axis aligned box on the unit sphere
// theta is elevation, phi is azimuth
class vsph_sph_box_2d
{
 public:
  //: Default constructor
  vsph_sph_box_2d();
  //: Specify units
  vsph_sph_box_2d(bool in_radians);

  ~vsph_sph_box_2d() {}

  bool in_radians() const {return in_radians_;}

  //: bounds on azimuth and elevation
  double min_phi(bool in_radians = true) const;
  double min_theta(bool in_radians = true) const;
  double max_phi(bool in_radians = true) const;
  double max_theta(bool in_radians = true) const;
  
  vsph_sph_point_2d min_point(bool in_radians = true) const;
  vsph_sph_point_2d max_point(bool in_radians = true) const;

  //: is box empty, i.e. no points have been added
  bool is_empty() const;

  //: add point to update box bounds
  void add( double theta, double phi, bool in_radians = true);
  void add( vsph_sph_point_2d const& pt){
    add(pt.theta_, pt.phi_, pt.in_radians_);}

  bool contains(double const& theta, double const& phi, bool in_radians = true) const;
  bool contains(vsph_sph_point_2d const& p) const;
  bool contains(vsph_sph_box_2d const& b) const;

  //: area on the surface of unit sphere
  double area() const;

  //: support for binary I/O
  void print(vcl_ostream& os, bool in_radians = true) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os);

  short version() const {return 1;}
 private:
  bool in_radians_;
  // pos[0]=theta, pos[1]=phi
  double min_pos_[2];
  double max_pos_[2];
};

vcl_ostream& operator<<(vcl_ostream& os, vsph_sph_box_2d const& p);

#endif
