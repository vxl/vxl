// This is oxl/mvl/PMatrix.h
#ifndef PMatrix_h_
#define PMatrix_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief General 3x4 perspective projection matrix
//
// A class to hold a perspective projection matrix and use it to
// perform common operations e.g. projecting point in 3d space to
// its image on the image plane
//
// \verbatim
//  Modifications
//     010796 AWF Implemented get_focal_point() - awf, july 96
//     011096 AWF Added caching vnl_svd<double>
//     260297 AWF Converted to use vnl_double_3x4
//     110397 PVR Added operator==
// \endverbatim

#include <vcl_iosfwd.h>

#include <vnl/algo/vnl_algo_fwd.h> // for vnl_svd
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3x4.h>
#include <vbl/vbl_ref_count.h>

class HomgPoint2D;
class HomgLine2D;
class HomgLineSeg2D;

class HomgPoint3D;
class HomgPlane3D;
class HomgLine3D;
class HomgLineSeg3D;
class HMatrix3D;
class HMatrix2D;

class PMatrix : public vbl_ref_count
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  PMatrix();
  PMatrix(vcl_istream&);
  PMatrix(const double *c_matrix);
  explicit PMatrix(const vnl_matrix<double>&);
  PMatrix(const vnl_matrix<double>& A, const vnl_vector<double>& a);
  PMatrix(const PMatrix&);
 ~PMatrix();

  static PMatrix read(const char* filename);
  static PMatrix read(vcl_istream&);

  // Operations----------------------------------------------------------------

  HomgPoint2D   project (const HomgPoint3D& X) const;
  HomgLine2D    project (const HomgLine3D& L) const;
  HomgLineSeg2D project (const HomgLineSeg3D& L) const;

  HomgPoint3D backproject_pseudoinverse (const HomgPoint2D& x) const;
  HomgLine3D  backproject (const HomgPoint2D& x) const;
  HomgPlane3D backproject (const HomgLine2D& l) const;

  PMatrix postmultiply(const HMatrix3D& H) const;
  PMatrix postmultiply(const vnl_matrix<double>& H) const;

  PMatrix premultiply(const HMatrix2D& H) const;
  PMatrix premultiply(const vnl_matrix<double>& H) const;

  vnl_svd<double>* svd() const; // mutable const
  void clear_svd();
  HomgPoint3D get_focal_point() const;
  HMatrix3D get_canonical_H() const;
  bool is_canonical(double tol = 0) const;

  bool is_behind_camera(const HomgPoint3D&);
  void flip_sign();
  bool looks_conditioned();
  void fix_cheirality();

  // Data Access---------------------------------------------------------------

  PMatrix& operator=(const PMatrix&);

  bool operator==(PMatrix const& p) const { return p_matrix_ == p.get_matrix(); }

  void get(vnl_matrix<double>* A, vnl_vector<double>* a) const;
  void set(const vnl_matrix<double>& A, const vnl_vector<double>& a);

  void get(vnl_vector<double>*, vnl_vector<double>*, vnl_vector<double>*) const;
  void set(const vnl_vector<double>&, const vnl_vector<double>&, const vnl_vector<double>&);

  double get (unsigned int row_index, unsigned int col_index) const;
  void get (double *c_matrix) const;
  void get (vnl_matrix<double>* p_matrix) const;

  void set (const double* p_matrix);
  void set (const double p_matrix [3][4]);
  void set (const vnl_matrix<double>& p_matrix);

  const vnl_matrix_ref<double> get_matrix() const { return p_matrix_; }
  // Can't implement this as it will blow the svd cache.
  // vnl_matrix<double>& get_matrix() { return p_matrix_; }

  // Utility Methods-----------------------------------------------------------
  bool read_ascii(vcl_istream& f);

  // Data Members--------------------------------------------------------------
 protected:
  vnl_double_3x4 p_matrix_;
  mutable vnl_svd<double>* svd_;
};

vcl_ostream& operator<<(vcl_ostream& s, const PMatrix& p);
vcl_istream& operator>>(vcl_istream& i, PMatrix& p);

//inline
//PMatrix operator*(const HMatrix3D& C, const PMatrix& P)
//{
//  return PMatrix(C.get_matrix() * P.get_matrix());
//}
//
inline
PMatrix operator*(const vnl_matrix<double>& C, const PMatrix& P)
{
  return PMatrix(C * P.get_matrix());
}

#endif // PMatrix_h_
