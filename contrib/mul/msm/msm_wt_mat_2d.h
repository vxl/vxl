#ifndef msm_wt_mat_2d_h_
#define msm_wt_mat_2d_h_
//:
// \file
// \brief Represents 2x2 symmetric matrix, used as weight matrix
// \author Tim Cootes

#include <iostream>
#include <iosfwd>
#include <string>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_fwd.h>
#include <vgl/vgl_vector_2d.h>

//: Represents 2x2 symmetric matrix, used as weight matrix
//  Stores the 3 unique elements and contains various utility
//  functions.
class msm_wt_mat_2d
{
 private:
  double m11_, m12_, m22_;
 public:

  // Default Constructor - set to identity
  msm_wt_mat_2d()
    : m11_(1.0),m12_(0.0),m22_(1.0) {}

  // Constructor (requires m11_>=0 and m22_>=0)
  msm_wt_mat_2d(double m11, double m12, double m22)
    : m11_(m11),m12_(m12),m22_(m22)
  { assert(m11_>=0); assert(m22_>=0);}

  // Destructor
  ~msm_wt_mat_2d() = default;

  //: Sets axis (eigenvector) of matrix and var along each
  //  Sets to s1*u*u' + s2*v*v', where u is the unit vector
  //  (u1 u2)/|u1,u2|, and v is the unit vector orthogonal to
  //  this.  u is then an eigenvector with associated eigenvalue s1,
  //  v is the other eigenvector, with eigenvalue s2
  void set_axes(double u1, double u2, double s1, double s2);

  double m11() const { return m11_; }
  double m12() const { return m12_; }
  double m21() const { return m12_; }
  double m22() const { return m22_; }

  // Determinant
  double det() const { return m11_*m22_ - m12_*m12_; }

  //: Calculate eigenvalues
  void eigen_values(double& EV1, double& EV2);

  //: Calculate eigenvector associated with largest eigenvalue.
  //  Other evec given by (-evec1.y(),evec1.x())
  void eigen_vector(vgl_vector_2d<double>& evec1, double& eval1, double& eval2);

  //: Calculates W2=T'WT where T is 2x2 matrix (a,-b;b,a)
  msm_wt_mat_2d transform_by(double a, double b) const;

  //: Compute (x y)W(x y)'
  double xWx(double x, double y) const
  { return x*x*m11_+y*y*m22_+2*x*y*m12_; }

  //: Post multiply by W
  msm_wt_mat_2d operator*(msm_wt_mat_2d& W) const;

  //: Multiply this by scalar
  msm_wt_mat_2d& operator*=(double s);

  //: Add W to this
  msm_wt_mat_2d& operator+=(const msm_wt_mat_2d& W);

  // Returns inverse (or pseudo inverse)
  msm_wt_mat_2d inverse() const;

  //: Print class to os
  void print_summary(std::ostream& os) const;

  //: Save class to binary file stream
  void b_write(vsl_b_ostream& bfs) const;

  //: Load class from binary file stream
  void b_read(vsl_b_istream& bfs);

  //: Equality test
  bool operator==(const msm_wt_mat_2d& wt_mat);
};


//: Binary file stream output operator for class reference
void vsl_b_write(vsl_b_ostream& bfs, const msm_wt_mat_2d& pts);

//: Binary file stream input operator for class reference
void vsl_b_read(vsl_b_istream& bfs, msm_wt_mat_2d& pts);

//: Stream output operator for class reference
std::ostream& operator<<(std::ostream& os,const msm_wt_mat_2d& pts);

//: Stream output operator for class reference
void vsl_print_summary(std::ostream& os,const msm_wt_mat_2d& pts);

#endif // msm_wt_mat_2d_h_
