#ifndef vsph_sph_box_2d_h_
#define vsph_sph_box_2d_h_
//:
// \file
// \brief An axis-aligned box on the unit sphere.
// \author J.L. Mundy
// \date February 1, 2013
// \verbatim
//  Modifications none
// \endverbatim

#include <iostream>
#include <vector>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsph/vsph_sph_point_2d.h>
#include <vgl/vgl_vector_3d.h>

//: An axis-aligned box on the unit sphere.
// \theta is elevation, \phi is azimuth
//
// Note that the creation of intervals on the azimuth circle is not
// well-defined. In contrast to intervals on the real line,
// there is an ambiguity as to which portion of the circle is
// the bounded set of angles. That is two points divide the circle
// into two arcs and the bounded set could be either one. Thus it is
// necessary to have three points. Points a and b define the two arcs and
// point c determines which arc contains the bounded set.
//
// To infer the bounded set from a sequence of points it is necessary to assume
// the initial three points are "compact," that is that they all lie in the
// smaller of the two arcs. That is |b - a|<180 and a<c<b . In this case,
// the predicate < means counter clockwise, i.e., a<b indicates that
// rotation to go from a to b is less than 180 and is counter-clockwise.
//
class vsph_sph_box_2d
{
 public:
  //: Default constructor
  vsph_sph_box_2d();
  //: Specify units
  vsph_sph_box_2d(bool in_radians);
  //: copy constructor
  vsph_sph_box_2d(const vsph_sph_box_2d& sbox);
  //: Constructor from three points.
  // Three points, pa, pb and pc, are needed to define an
  // unambiguous order on azimuth having a cut at +-180.
  // Two of the points, pa and pb, bound the interval and the third
  // point, c, defines which complementary arc of the circle is "in" the box.
  // pa < pb by definition. pc lies on the  smaller or larger arc to
  // break the ambiguity
  vsph_sph_box_2d(vsph_sph_point_2d const& pa, vsph_sph_point_2d const& pb,
                  vsph_sph_point_2d const& pc);

  ~vsph_sph_box_2d() = default;

  void set(double min_theta, double max_theta, double a_phi, double b_phi,
           double c_phi, bool in_radians = true);

  vsph_sph_box_2d& operator= (const vsph_sph_box_2d & rhs);

  //: the angle units (radians or degrees) maintained by *this box
  bool in_radians() const {return in_radians_;}

  //: bounds on azimuth and elevation, if in_radians == false.
  // the return is in degrees.
  double min_phi(bool in_radians = true) const;
  double min_theta(bool in_radians = true) const;
  double max_phi(bool in_radians = true) const;
  double max_theta(bool in_radians = true) const;

  double a_phi(bool in_radians = true) const;
  double b_phi(bool in_radians = true) const;
  double c_phi(bool in_radians = true) const;

  vsph_sph_point_2d min_point(bool in_radians = true) const;
  vsph_sph_point_2d max_point(bool in_radians = true) const;

  //: is box empty, i.e. no points have been added
  bool is_empty() const;

  //: add point to update box bounds.
  // assumes the box already has the three point basis
  // or the added point sequence is from a "compact" set on the circle
  void add( double theta, double phi, bool in_radians = true);
  void add( vsph_sph_point_2d const& pt) { add(pt.theta_, pt.phi_, pt.in_radians_); }
  //: are two boxes the same box (same spherical domain)
  bool operator==(const vsph_sph_box_2d& other) const;

  //: does the box have enough added points to uniquely define interval bounds
  bool defined() const;

  //: is an azimuth angle contained in the current bounded azimuth interval
  bool in_interval(double phi, bool in_radians=true) const;
  //: does the box contain the specified spherical point ?
  bool contains(double const& theta, double const& phi, bool in_radians = true) const;
  bool contains(vsph_sph_point_2d const& p) const;

  //: does *this  box fully contain box b
  bool contains(vsph_sph_box_2d const& b) const;

  //: area on the surface of unit sphere
  double area() const;

  //: the spherical point corresponding to the center of the box
  vsph_sph_point_2d center(bool in_radians = true) const;

  //: transform the box on the unit sphere
  vsph_sph_box_2d transform(double t_theta, double t_phi, double scale,
                            bool in_radians) const;
  vsph_sph_box_2d transform(double t_theta,
                            double t_phi, double scale,
                            double theta_c,double phi_c,
                            bool in_radians) const;

  //:subdivide box into potentially a 2x2 set of sub-boxes. Don't subdivide
  // along a given axis if resulting angular range is less than min_ang
  bool sub_divide(std::vector<vsph_sph_box_2d>& sub_boxes,
                  double min_ang = 0.035) const;

  //: decompose box into approximately planar quadrilaterals
  void planar_quads(std::vector<vgl_vector_3d<double> >& verts,
                    std::vector<std::vector<int> >& quads,
                    double tol = 0.01) const;

  //: display the box as a set of planar quadrilaterals in vrml
  void display_box(std::ostream& os, float r, float g, float b,
                   double tol = 0.01,
                   double factor =1.0) const;

  //: display a set of boxes
  static void display_boxes(std::string const& path,
                            std::vector<vsph_sph_box_2d> const& boxes,
                            std::vector<std::vector<float> > colors,
                            double tol = 0.01,
                            double factor =1.0);

  //: support for binary I/O
  void print(std::ostream& os, bool in_radians = false) const;

  void b_read(vsl_b_istream& is);

  void b_write(vsl_b_ostream& os);

  short version() const {return 1;}
 private:
  //: the value of pi in the units of *this box
  double pye() const;

  //: reduce phi to the range +-180 (assumed in *this angle units)
  double reduce_phi(double phi) const;

  //: bounds of ccw traversal of phi interval in *this angle units
  void phi_bounds(double& phi_start, double& phi_end) const;

  //: assign phi_a, phi_b so that b is ccw of a
  void b_ccw_a();
  //: assign comparisons
  void set_comparisons();
  //: update the current theta bounds
  void update_theta(double th);
  //: the azimuth angle ph is outside the current interval so extend it
  bool extend_interval(double ph);
  //: the angle units of *this box
  bool in_radians_;
  //: the three azimuth angles that define an unambigous bounded set
  // a and b define two circular intervals and c determines the bounded interval
  double a_phi_, b_phi_, c_phi_;
  double min_phi_, max_phi_;
  //: the bounds on elevation
  double min_th_, max_th_;
  bool a_ge_zero_, b_le_zero_, b_lt_zero_;
  bool c_gt_a_, c_lt_b_;
  bool c_le_pi_, c_ge_mpi_;
  bool c_ge_zero_;
};

//: return a box that represents the intersection of two boxes (could be empty)
// note that it is possible to have two disjoint intervals in phi (2 boxes)
bool intersection(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2,
                  std::vector<vsph_sph_box_2d>& boxes);

//: return the area of the intersection
double intersection_area(vsph_sph_box_2d const& b1, vsph_sph_box_2d const& b2);

std::ostream& operator<<(std::ostream& os, vsph_sph_box_2d const& p);

#endif //vsph_sph_box_2d_h_
