#ifndef HMatrix3D_h_
#define HMatrix3D_h_
#ifdef __GNUC__
#pragma interface
#endif
//:
// \file
//
// A class to hold a 3D projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
#include <vnl/vnl_double_4x4.h>
#include <mvl/HomgPoint3D.h>
#include <mvl/HomgLine3D.h>
#include <mvl/HomgLineSeg3D.h>

class PMatrix;

class HMatrix3D : public vnl_double_4x4
{
  typedef vnl_double_4x4 Base;
public:
  HMatrix3D();
  HMatrix3D(const HMatrix3D& M);
  HMatrix3D(const vnl_matrix<double>& M);
  HMatrix3D(const vnl_matrix<double>& M, const vnl_vector<double>& m);
  HMatrix3D(const double* t_matrix);
  HMatrix3D(vcl_istream&);
 ~HMatrix3D();

  // Operations----------------------------------------------------------------

  HomgPoint3D transform(const HomgPoint3D& x1) const;
  HomgLine3D transform(const HomgLine3D& l1) const;

  bool load(vcl_istream&);

  // Data Access---------------------------------------------------------------

  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;

  // @deprecated
  vnl_double_4x4& asMatrix () { return *this; }

  // @deprecated
  const vnl_double_4x4& get_matrix () const { return *this; }

  HMatrix3D Inverse () const;
};

PMatrix operator* (const PMatrix&, const HMatrix3D& H);

// stream I/O
vcl_ostream &operator<<(vcl_ostream &,HMatrix3D const &);
vcl_istream& operator>>(vcl_istream &,HMatrix3D       &);

#endif // HMatrix3D_h_
