// This is oxl/mvl/PMatrix.h
#ifndef PMatrix_h_
#define PMatrix_h_
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
//     110397 PVr Added operator==
//     221002 Peter Vanroose - added vgl_homg_point_2d interface
//     231002 Peter Vanroose - using fixed 3x4 matrices throughout
// \endverbatim

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_algo_fwd.h> // for vnl_svd
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_double_4x4.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h> // for p_matrix_ * vgl_homg_point_3d
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
  PMatrix(std::istream&);
  PMatrix(const double *c_matrix);
  explicit PMatrix(vnl_double_3x4 const&);
  PMatrix(const vnl_matrix<double>& A, const vnl_vector<double>& a);
  PMatrix(const PMatrix&);
 ~PMatrix() override;

  static PMatrix read(const char* filename);
  static PMatrix read(std::istream&);

  // Operations----------------------------------------------------------------

  HomgPoint2D   project(const HomgPoint3D& X) const;
  HomgLine2D    project(const HomgLine3D& L) const;
  HomgLineSeg2D project(const HomgLineSeg3D& L) const;

  HomgPoint3D backproject_pseudoinverse(const HomgPoint2D& x) const;
  HomgLine3D  backproject(const HomgPoint2D& x) const;
  HomgPlane3D backproject(const HomgLine2D& l) const;

  //: Return the image point which is the projection of the specified 3D point X
  vgl_homg_point_2d<double>   project(vgl_homg_point_3d<double> const& X) const { return p_matrix_ * X; }
  //: Return the image line which is the projection of the specified 3D line L
  vgl_homg_line_2d<double>    project(vgl_homg_line_3d_2_points<double> const& L) const;
  //: Return the image linesegment which is the projection of the specified 3D linesegment L
  vgl_line_segment_2d<double> project(vgl_line_segment_3d<double> const& L) const;

  //: Return the 3D point $\vec X$ which is $\vec X = P^+ \vec x$.
  vgl_homg_point_3d<double> backproject_pseudoinverse(vgl_homg_point_2d<double> const& x) const;
  //: Return the 3D line which is the backprojection of the specified image point, x.
  vgl_homg_line_3d_2_points<double>  backproject(vgl_homg_point_2d<double> const& x) const;
  //: Return the 3D plane which is the backprojection of the specified line l in the image
  vgl_homg_plane_3d<double> backproject(vgl_homg_line_2d<double> const& l) const;

  //: post-multiply this projection matrix with a HMatrix3D
  PMatrix postmultiply(vnl_double_4x4 const& H) const;

  //: pre-multiply this projection matrix with a HMatrix2D
  PMatrix premultiply(vnl_double_3x3 const& H) const;

  vnl_svd<double>* svd() const; // mutable const
  void clear_svd() const;
  HomgPoint3D get_focal_point() const;
  vgl_homg_point_3d<double> get_focal() const;
  HMatrix3D get_canonical_H() const;
  bool is_canonical(double tol = 0) const;

  bool is_behind_camera(const HomgPoint3D&);
  bool is_behind_camera(vgl_homg_point_3d<double> const&);
  void flip_sign();
  bool looks_conditioned();
  void fix_cheirality();

  // Data Access---------------------------------------------------------------

  PMatrix& operator=(const PMatrix&);

  bool operator==(PMatrix const& p) const { return p_matrix_ == p.get_matrix(); }

  void get(vnl_matrix<double>* A, vnl_vector<double>* a) const;
  void get(vnl_double_3x3* A, vnl_double_3* a) const;
  void set(vnl_double_3x3 const& A, vnl_double_3 const& a);

  void get_rows(vnl_vector<double>*, vnl_vector<double>*, vnl_vector<double>*) const;
  void get_rows(vnl_double_4*, vnl_double_4*, vnl_double_4*) const;
  void set_rows(const vnl_vector<double>&, const vnl_vector<double>&, const vnl_vector<double>&);

  double get(unsigned int row_index, unsigned int col_index) const;
  void get(double *c_matrix) const;
  void get(vnl_matrix<double>* p_matrix) const;
  void get(vnl_double_3x4* p_matrix) const;

  void set(const double* p_matrix);
  void set(const double p_matrix [3][4]);
  void set(const vnl_matrix<double>& p_matrix);
  void set(vnl_double_3x4 const& p_matrix);

  const vnl_double_3x4& get_matrix() const { return p_matrix_; }

  // Utility Methods-----------------------------------------------------------
  bool read_ascii(std::istream& f);

  // Data Members--------------------------------------------------------------
 protected:
  vnl_double_3x4 p_matrix_;
  mutable vnl_svd<double>* svd_;
};

std::ostream& operator<<(std::ostream& s, const PMatrix& p);
std::istream& operator>>(std::istream& i, PMatrix& p);

inline
PMatrix operator*(vnl_double_3x3 const& C, const PMatrix& P)
{
  return PMatrix(C * P.get_matrix());
}

#endif // PMatrix_h_
