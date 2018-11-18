// This is mul/mbl/mbl_thin_plate_spline_2d.h
#ifndef mbl_thin_plate_spline_2d_h_
#define mbl_thin_plate_spline_2d_h_
//:
// \file
// \brief Construct thin plate spline to map 2D to 2D
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
//: Construct thin plate spline to map 2D to 2D.
// I.e. does some mapping (x',y') = f(x,y). (See Booksteins work, e.g. IPMI 1993)
// The warp is `guided' by a set of
// landmarks p(0) .. p(n-1) in the source plane which are to be
// mapped to a (possibly deformed) set q(0)..q(n-1) in the destination.
// Thus the mapping is constrained so that f(p(i)) = q(i) for i = 0..n-1.
// The points are given to the build() function to set up the object.
//
// If one wishes to map a set of source points to multiple target points,
// use set_source_pts(src_pts);  then build(target_pts); for each target set.
//
// \verbatim
// std::vector<vgl_point_2d<double> > src_pts(n_points),dest_pts(n_points);
//
// Fill src_pts and dest_pts
// .....
//
// Construct spline object
// mbl_thin_plate_spline_2d tps;
// tps.build(src_pts,dest_pts);
//
// // Apply to point p:
// vgl_point_3d<double> p(1,2);
//
// vgl_point_2d<double> new_p = tps(p);
// \endverbatim
class mbl_thin_plate_spline_2d
{
  vnl_vector<double> Wx_,Wy_;
  double Ax0_, AxX_, AxY_;
  double Ay0_, AyX_, AyY_;
  double energy_x_,energy_y_;

  bool return_pure_affine_;

  std::vector<vgl_point_2d<double> > src_pts_;

    //: Used to estimate weights in set_source_points()
  vnl_matrix<double> L_inv_;

    //: Build from small number of points
  void build_pure_affine(const std::vector<vgl_point_2d<double> >& source_pts,
                         const std::vector<vgl_point_2d<double> >& dest_pts);

   //: Set parameters from vectors
  void set_params(const vnl_vector<double>& W1,
                  const vnl_vector<double>& W2);

  void set_up_rhs(vnl_vector<double>& Bx,
                  vnl_vector<double>& By,
                  const std::vector<vgl_point_2d<double> >& dest_pts);

   //: Compute spline-bending energy
  void compute_energy(vnl_vector<double>& W1,
                      vnl_vector<double>& W2,
                      const vnl_matrix<double>& L);

 public:

    //: Dflt ctor
  mbl_thin_plate_spline_2d();

    //: Destructor
  virtual ~mbl_thin_plate_spline_2d();

    //: Sets up internal transformation to map source_pts onto dest_pts
  void build(const std::vector<vgl_point_2d<double> >& source_pts,
             const std::vector<vgl_point_2d<double> >& dest_pts,
             bool compute_the_energy=false);

    //: Define source point positions
    //  Performs pre-computations so that build(dest_points) can be
    //  called multiple times efficiently
  void set_source_pts(const std::vector<vgl_point_2d<double> >& source_pts);

  //: Return current source points
  const std::vector<vgl_point_2d<double> >& src_pts() const
  { return src_pts_; }


    //: Sets up internal transformation to map source_pts onto dest_pts
  void build(const std::vector<vgl_point_2d<double> >& dest_pts);

       //: Return transformed version of (x,y)
  vgl_point_2d<double>  operator()(double x, double y) const;

       //: Return transformed version of (x,y)
  vgl_point_2d<double>  operator()(const vgl_point_2d<double>&  p) const
  { return operator()(p.x(),p.y()); }

    //: Bending energy of X component (zero for pure affine)
    //  A measure of total amount of non-linear deformation
  double bendingEnergyX() const { return energy_x_; }

    //: Bending energy of X component (zero for pure affine)
    //  A measure of total amount of non-linear deformation
  double bendingEnergyY() const { return energy_y_; }

  //:
  // If this parameter is set to true, then only global affine part
  // of the currently computed transformation is used
  void set_pure_affine(bool val) { return_pure_affine_ = val; }

    //: Version number for I/O
  short version_no() const;

    //: Print class to os
  void print_summary(std::ostream& os) const;

    //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

    //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

    //: Comparison operator
  bool operator==(const mbl_thin_plate_spline_2d& tps) const;
};

  //: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const mbl_thin_plate_spline_2d& b);

  //: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, mbl_thin_plate_spline_2d& b);

  //: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const mbl_thin_plate_spline_2d& b);

#endif
