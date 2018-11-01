// This is oxl/mvl/HMatrix2D.h
#ifndef _HMatrix2D_h
#define _HMatrix2D_h
//:
//  \file
// \brief 3x3 plane-to-plane projectivity
//
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//    23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h> // t12_matrix_ * vgl_homg_point_2d
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgLineSeg2D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.

class HMatrix2D
{
  // Data Members--------------------------------------------------------------
  vnl_double_3x3 t12_matrix_;

 public:

  //: Flags for reduced H matrices
  enum Type {
    Euclidean,
    Similarity,
    Affine,
    Projective
  };

  // Constructors/Initializers/Destructors-------------------------------------

  HMatrix2D();
  HMatrix2D(const HMatrix2D& M);
  HMatrix2D(vnl_double_3x3 const& M);
  HMatrix2D(const double* t_matrix);
  HMatrix2D(std::istream& s);
  HMatrix2D(char const* filename);
 ~HMatrix2D();

  // Operations----------------------------------------------------------------

  // Deprecated Methods:
  HomgPoint2D transform_to_plane2(const HomgPoint2D& x1) const;
  vgl_homg_point_2d<double> transform_to_plane2(vgl_homg_point_2d<double> const& x1) const
    { return t12_matrix_ * x1; }
  HomgLine2D  transform_to_plane1(const HomgLine2D&  l2) const;
  vgl_homg_line_2d<double>  transform_to_plane1(vgl_homg_line_2d <double> const& l2) const
    { return t12_matrix_.transpose() * l2; }
  HomgLine2D  transform_to_plane2(const HomgLine2D&  l1) const;
  // Note that this calculates the inverse of $H$ on every call.
  vgl_homg_line_2d<double>  transform_to_plane2(vgl_homg_line_2d <double> const& l1) const
    { return this->get_inverse().get_matrix().transpose() * l1; }
  HomgPoint2D transform_to_plane1(const HomgPoint2D& x2) const;
  // Note that this calculates the inverse of $H$ on every call.
  vgl_homg_point_2d<double> transform_to_plane1(vgl_homg_point_2d<double> const& x2) const
    { return this->get_inverse().get_matrix() * x2; }

  HomgPoint2D operator*(const HomgPoint2D& x1) const { return transform_to_plane2(x1); }
  vgl_homg_point_2d<double> operator*(vgl_homg_point_2d<double> const& x1) const { return transform_to_plane2(x1); }
  HomgPoint2D operator()(const HomgPoint2D& p) const { return transform_to_plane2(p); }
  vgl_homg_point_2d<double> operator()(vgl_homg_point_2d<double> const& p) const { return transform_to_plane2(p); }
  HomgLine2D preimage(const HomgLine2D& l) const { return this->transform_to_plane1(l); }
  vgl_homg_line_2d<double> preimage(vgl_homg_line_2d<double> const& l) const { return this->transform_to_plane1(l); }

  // WARNING. these cost one vnl_svd<double>, so are here for convenience only :
  HomgPoint2D preimage(const HomgPoint2D& p) const { return this->get_inverse().transform_to_plane2(p); }
  vgl_homg_point_2d<double> preimage(vgl_homg_point_2d<double> const& p) const {return this->get_inverse().transform_to_plane2(p);}
  HomgLine2D operator()(const HomgLine2D& l) const { return this->get_inverse().transform_to_plane1(l); }
  vgl_homg_line_2d<double> operator()(vgl_homg_line_2d<double> const& l) const {return this->get_inverse().transform_to_plane1(l);}

  vnl_double_4 transform_bounding_box(double x0, double y0, double x1, double y1);

  //: Composition
  HMatrix2D operator*(const HMatrix2D& H2) { return HMatrix2D(t12_matrix_ * H2.t12_matrix_); }

  // Data Access---------------------------------------------------------------

  double get(unsigned int row_index, unsigned int col_index) const;
  void get(double *t_matrix) const;
  void get(vnl_matrix<double>* t_matrix) const;
  const vnl_double_3x3& get_matrix() const { return t12_matrix_; }
  HMatrix2D get_inverse() const;

  void set_identity();
  void set(const double *t_matrix);
  void set(vnl_double_3x3 const& t_matrix);

  bool read(std::istream& s);
  bool read(char const* filename);
};

std::ostream& operator<<(std::ostream& s, const HMatrix2D& H);
std::istream& operator>>(std::istream& s, HMatrix2D& H);

#endif // _HMatrix2D_h
