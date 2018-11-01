// This is oxl/mvl/HMatrix3D.h
#ifndef HMatrix3D_h_
#define HMatrix3D_h_
//:
// \file
//
// A class to hold a 3D projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_3d interface
//    23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
// \endverbatim

#include <iostream>
#include <iosfwd>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h> // matrix * vgl_homg_point_3d
#include <mvl/HomgPoint3D.h>
#include <mvl/HomgLine3D.h>
#include <mvl/HomgLineSeg3D.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class PMatrix;

class HMatrix3D : public vnl_double_4x4
{
  typedef vnl_double_4x4 Base;
 public:
  HMatrix3D();
  HMatrix3D(const HMatrix3D& M);
  HMatrix3D(vnl_double_4x4 const& M);
  HMatrix3D(vnl_double_3x3 const& M, vnl_double_3 const& m);
  HMatrix3D(const double* t_matrix);
  HMatrix3D(std::istream&);
 ~HMatrix3D();

  // Operations----------------------------------------------------------------

  HomgPoint3D transform(const HomgPoint3D& x1) const;
  vgl_homg_point_3d<double> transform(const vgl_homg_point_3d<double>& x1) const
    { return (*this) * x1; }
  HomgLine3D transform(const HomgLine3D& l1) const;

  bool load(std::istream&);

  // Data Access---------------------------------------------------------------

  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;

  HMatrix3D get_inverse() const;
};

PMatrix operator* (const PMatrix&, const HMatrix3D& H);

// stream I/O
std::ostream& operator<<(std::ostream &,HMatrix3D const &);
std::istream& operator>>(std::istream &,HMatrix3D       &);

#endif // HMatrix3D_h_
