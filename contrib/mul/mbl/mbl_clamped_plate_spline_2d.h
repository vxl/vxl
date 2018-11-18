#ifndef mbl_clamped_plate_spline_2d_h_
#define mbl_clamped_plate_spline_2d_h_

//:
// \file
// \brief Construct clamped plate spline to map 2D to 2D
// \author Tim Cootes

#include <vector>
#include <iostream>
#include <iosfwd>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//=======================================================================
//: Construct clamped plate spline to map 2D points in unit disk.
// I.e. does some mapping (x',y') = f(x,y).
//
// Acts only within unit disk.  The deformation and its derivatives are
// zero at the unit circle.
//
// For more details, see "Measuring Geodesic Distances on the Space of
// Bounded Diffeomorphisms", C.Twining, S.Marsland et.al.
//
// The warp is `guided' by a set of
// landmarks p(0) .. p(n-1) in the source plane which are to be
// mapped to a (possibly deformed) set q(0)..q(n-1) in the destination.
// Thus the mapping is constrained so that f(p(i)) = q(i) for i = 0..n-1.
// The points are given to the build() function to set up the object.
//
// If one wishes to map a set of source points to multiple target points,
// use set_source_pts(src_pts);  then build(target_pts); for each target set.
class mbl_clamped_plate_spline_2d {
private:
  vnl_vector<double> Wx_,Wy_;

  std::vector<vgl_point_2d<double> > src_pts_;

    //: Used to estimate weights in set_source_points()
  vnl_matrix<double> L_inv_;

    //: Check that all points are inside unit circle
  bool all_in_unit_circle(const std::vector<vgl_point_2d<double> >& pts);

   //: Set parameters from vectors
  void set_params(const vnl_vector<double>& Wx,
                  const vnl_vector<double>& Wy);

  void set_up_rhs(vnl_vector<double>& Bx,
                  vnl_vector<double>& By,
                  const std::vector<vgl_point_2d<double> >& src_pts,
                  const std::vector<vgl_point_2d<double> >& dest_pts);

public:

    //: Dflt ctor
  mbl_clamped_plate_spline_2d();

    //: Destructor
  virtual ~mbl_clamped_plate_spline_2d();

    //: Sets up internal transformation to map source_pts onto dest_pts
  void build(const std::vector<vgl_point_2d<double> >& source_pts,
             const std::vector<vgl_point_2d<double> >& dest_pts);

    //: Define source point positions
    //  Performs pre-computations so that build(dest_points) can be
    //  called multiple times efficiently
  void set_source_pts(const std::vector<vgl_point_2d<double> >& source_pts);

    //: Sets up internal transformation to map source_pts onto dest_pts
  void build(const std::vector<vgl_point_2d<double> >& dest_pts);

       //: Return transformed version of (x,y)
  vgl_point_2d<double>  operator()(double x, double y) const;

       //: Return transformed version of (x,y)
  vgl_point_2d<double>  operator()(const vgl_point_2d<double>&  p) const
  { return operator()(p.x(),p.y()); }

    //: Version number for I/O
  short version_no() const;

    //: Print class to os
  void print_summary(std::ostream& os) const;

    //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

    //: Comparison operator
  bool operator==(const mbl_clamped_plate_spline_2d& tps) const;
};

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_clamped_plate_spline_2d& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_clamped_plate_spline_2d& b);

  //: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mbl_clamped_plate_spline_2d& b);

#endif
