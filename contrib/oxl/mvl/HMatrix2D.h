// This is oxl/mvl/HMatrix2D.h
#ifndef _HMatrix2D_h
#define _HMatrix2D_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
//  \file
// \brief 3x3 plane-to-plane projectivity
//
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_4.h>
#include <mvl/HomgPoint2D.h>
#include <mvl/HomgLine2D.h>
#include <mvl/HomgLineSeg2D.h>
#include <vcl_iosfwd.h>

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
  HMatrix2D(const vnl_matrix<double>& M);
  HMatrix2D(const double* t_matrix);
  HMatrix2D(vcl_istream& s);
  HMatrix2D(char const* filename);
 ~HMatrix2D();

  // Operations----------------------------------------------------------------

  // Deprecated Methods:
  HomgPoint2D transform_to_plane2(const HomgPoint2D& x1) const;
  HomgLine2D  transform_to_plane1(const HomgLine2D& l2) const;
  HomgLine2D  transform_to_plane2(const HomgLine2D& l1) const;
  HomgPoint2D transform_to_plane1(const HomgPoint2D& x2) const;

  HomgPoint2D operator*(const HomgPoint2D& x1) const { return transform_to_plane2(x1); }
  HomgPoint2D operator()(const HomgPoint2D& p) const { return transform_to_plane2(p); }
  HomgLine2D preimage(const HomgLine2D& l) const { return this->transform_to_plane1(l); }

  // WARNING. these cost one vnl_svd<double>, so are here for convenience only :
  HomgPoint2D preimage(const HomgPoint2D& p) const { return this->get_inverse().transform_to_plane2(p); }
  HomgLine2D operator()(const HomgLine2D& l) const { return this->get_inverse().transform_to_plane1(l); }

  vnl_double_4 transform_bounding_box(double x0, double y0, double x1, double y1);

  // HomgPoint2D transform_to_plane1(const HomgPoint2D& x2) const;
  // HomgLine2D transform_to_plane2(const HomgLine2D& l1) const;

  //: Composition
  HMatrix2D operator*(const HMatrix2D& H2) { return HMatrix2D(t12_matrix_ * H2.t12_matrix_); }

  // Data Access---------------------------------------------------------------

  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *t_matrix) const;
  void get (vnl_matrix<double>* t_matrix) const;
  const vnl_double_3x3& get_matrix () const { return t12_matrix_; }
  HMatrix2D get_inverse() const;

  void set_identity();
  void set (const double *t_matrix);
  void set (const vnl_matrix<double>& t_matrix);

  bool read(vcl_istream& s);
  bool read(char const* filename);
};

vcl_ostream& operator << (vcl_ostream& s, const HMatrix2D& H);
vcl_istream& operator >> (vcl_istream& s, HMatrix2D& H);

#endif // _HMatrix2D_h
