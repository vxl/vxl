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

#include <vnl/vnl_double_2x2.h>
#include <mvl/HomgPoint1D.h>
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
  HMatrix1D(const vnl_matrix<double>& M);
  HMatrix1D(const double* t_matrix);
  HMatrix1D(vcl_istream& s);
 ~HMatrix1D();
  static HMatrix1D read(char const* filename);
  static HMatrix1D read(vcl_istream&);

  // Operations----------------------------------------------------------------

  // deprecated. also misnomers :
  HomgPoint1D transform_to_plane2(const HomgPoint1D& x1) const;
  HomgPoint1D transform_to_plane1(const HomgPoint1D& x2) const;

  HomgPoint1D operator()(const HomgPoint1D& x1) const;
  HomgPoint1D preimage(const HomgPoint1D& x2) const;

  // Data Access---------------------------------------------------------------

  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;
  const vnl_double_2x2& get_matrix () const { return t12_matrix_; }
  const vnl_double_2x2& get_inverse () const { return t21_matrix_; }

  void set (const double *t_matrix);
  void set (const vnl_matrix<double>& t_matrix);
  void set_inverse (const vnl_matrix<double>& t21_matrix);
};

vcl_ostream& operator << (vcl_ostream& s, const HMatrix1D& H);
vcl_istream& operator >> (vcl_istream& s, HMatrix1D& H);

#endif // HMatrix1D_h_
