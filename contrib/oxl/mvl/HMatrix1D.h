// This is oxl/mvl/HMatrix1D.h
#ifndef HMatrix1D_h_
#define HMatrix1D_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
//
// A class to hold a line-to-line projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_1d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
// \endverbatim

#include <vnl/vnl_double_2x2.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_iosfwd.h>

class HMatrix1D
{
  // Data Members--------------------------------------------------------------
  vnl_double_2x2 t12_matrix_;
  vnl_double_2x2 t21_matrix_;

 public:
  // Constructors/Initializers/Destructors-------------------------------------

  HMatrix1D();
  HMatrix1D(const HMatrix1D& M);
  HMatrix1D(const HMatrix1D&,const HMatrix1D&);// product of two HMatrix1Ds
  HMatrix1D(vnl_double_2x2 const& M);
  HMatrix1D(const double* t_matrix);
  HMatrix1D(vcl_istream& s);
 ~HMatrix1D();
  static HMatrix1D read(char const* filename);
  static HMatrix1D read(vcl_istream&);

  // Operations----------------------------------------------------------------

  // deprecated. also misnomers :
  vgl_homg_point_1d<double> transform_to_plane2(const vgl_homg_point_1d<double>& x1) const;
  vgl_homg_point_1d<double> transform_to_plane1(const vgl_homg_point_1d<double>& x2) const;

  vgl_homg_point_1d<double> operator()(const vgl_homg_point_1d<double>& x1) const;
  vgl_homg_point_1d<double> preimage(const vgl_homg_point_1d<double>& x2) const;

  // Data Access---------------------------------------------------------------

  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;
  const vnl_double_2x2& get_matrix () const { return t12_matrix_; }
  const vnl_double_2x2& get_inverse () const { return t21_matrix_; }

  void set (const double *t_matrix);
  void set (vnl_double_2x2 const& t_matrix);
  void set_inverse (vnl_double_2x2 const& t21_matrix);
};

vcl_ostream& operator << (vcl_ostream& s, const HMatrix1D& H);
vcl_istream& operator >> (vcl_istream& s, HMatrix1D& H);

#endif // HMatrix1D_h_
