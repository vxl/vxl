// This is core/vgl/algo/vgl_h_matrix_3d.h
#ifndef vgl_h_matrix_3d_h_
#define vgl_h_matrix_3d_h_
//:
// \file
// \brief 4x4 3D-to-3D projectivity
//
// A class to hold a 3D projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//   22 Mar 2003 - J. L. Mundy  - prep for moving to vgl
//   31 Jul 2010 - Peter Vanroose - made more similar to 1d and 2d variants
//   24 Oct 2010 - Peter Vanroose - mutators and setters now return *this
//   27 Oct 2010 - Peter Vanroose - moved Doxygen docs from .hxx to .h
//   26 Jul 2011 - Peter Vanroose - added correlation(),set_affine(),is_identity()
// \endverbatim

#include <vector>
#include <iosfwd>
#include <vnl/vnl_fwd.h> // for vnl_vector_fixed<T,3>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/vgl_pointset_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// A class to hold a 3-d projective transformation matrix
// and to perform common operations using it e.g. transform point.
template <class T>
class vgl_h_matrix_3d
{
protected:
  vnl_matrix_fixed<T, 4, 4> t12_matrix_;

public:
  vgl_h_matrix_3d() = default;
  ~vgl_h_matrix_3d() = default;
  //: Copy constructor
  vgl_h_matrix_3d(const vgl_h_matrix_3d<T> & M)
    : t12_matrix_(M.get_matrix())
  {}
  //: Constructor from a 4x4 matrix, and implicit cast from vnl_matrix_fixed<T,4,4>
  vgl_h_matrix_3d(const vnl_matrix_fixed<T, 4, 4> & M)
    : t12_matrix_(M)
  {}
  //: Construct an affine vgl_h_matrix_3d from 3x3 M and 3x1 m.
  vgl_h_matrix_3d(const vnl_matrix_fixed<T, 3, 3> & M, const vnl_vector_fixed<T, 3> & m);
  //: Constructor from 4x4 row-storage C-array
  explicit vgl_h_matrix_3d(const T * M)
    : t12_matrix_(M)
  {}
  //: Load from ASCII std::istream.
  explicit vgl_h_matrix_3d(std::istream & s);
  //: Load from file
  explicit vgl_h_matrix_3d(const char * filename);
  //: Constructor - calculate homography between two sets of 3D points (minimum 5)
  vgl_h_matrix_3d(const std::vector<vgl_homg_point_3d<T>> & points1, const std::vector<vgl_homg_point_3d<T>> & points2);

  inline explicit
  operator vnl_matrix<T>() const
  {
    return this->t12_matrix_.as_matrix();
  }

  // Operations----------------------------------------------------------------

  //: Return the transformed point given by $q = {\tt H} p$
  vgl_homg_point_3d<T>
  operator()(const vgl_homg_point_3d<T> & p) const;
  //: Return the transformed point given by $q = {\tt H} p$
  vgl_homg_point_3d<T>
  operator*(const vgl_homg_point_3d<T> & p) const
  {
    return (*this)(p);
  }

  bool
  operator==(const vgl_h_matrix_3d<T> & M) const
  {
    return t12_matrix_ == M.get_matrix();
  }

  //: Return the preimage of a transformed plane: $m = {\tt H} l$
  vgl_homg_plane_3d<T>
  preimage(const vgl_homg_plane_3d<T> & l) const;
  vgl_homg_plane_3d<T>
  correlation(const vgl_homg_point_3d<T> & p) const;
  vgl_homg_point_3d<T>
  correlation(const vgl_homg_plane_3d<T> & l) const;


  //: operate directly on Euclidean points for convenience (no ideal points allowed)

  vgl_point_3d<T>
  operator()(const vgl_point_3d<T> & p) const
  {
    vgl_homg_point_3d<T> hp(p);
    return (*this)(hp);
  }

  vgl_point_3d<T>
  operator*(const vgl_point_3d<T> & p) const
  {
    return (*this)(p);
  }

  vgl_pointset_3d<T>
  operator()(const vgl_pointset_3d<T> & ptset) const;

  // the following require computing the inverse homography

  //: Return the preimage of a transformed point: $p = {\tt H}^{-1} q$
  // (requires an inverse)
  vgl_homg_point_3d<T>
  preimage(const vgl_homg_point_3d<T> & q) const;
  vgl_pointset_3d<T>
  preimage(const vgl_pointset_3d<T> & ptset) const;

  //: Return the transformed plane given by $m = {\tt H}^{-1} l$
  // (requires an inverse)
  vgl_homg_plane_3d<T>
  operator()(const vgl_homg_plane_3d<T> & l) const;
  //: Return the transformed plane given by $m = {\tt H}^{-1} l$
  // (requires an inverse)
  vgl_homg_plane_3d<T>
  operator*(const vgl_homg_plane_3d<T> & l) const
  {
    return (*this)(l);
  }

  //: composition (*this) * H
  vgl_h_matrix_3d<T>
  operator*(const vgl_h_matrix_3d<T> & H) const
  {
    return vgl_h_matrix_3d<T>(t12_matrix_ * H.t12_matrix_);
  }

  // Data Access---------------------------------------------------------------

  //: Return the 4x4 homography matrix
  const vnl_matrix_fixed<T, 4, 4> &
  get_matrix() const
  {
    return t12_matrix_;
  }
  //: Fill M with contents of the 4x4 homography matrix
  void
  get(vnl_matrix_fixed<T, 4, 4> * M) const;
  //: Fill M with contents of the 4x4 homography matrix
  void
  get(T * M) const;
  //: Return an element from the 4x4 homography matrix
  T
  get(unsigned int row_index, unsigned int col_index) const;
  //: Return the inverse homography
  vgl_h_matrix_3d
  get_inverse() const;

  //: Set an element of the 4x4 homography matrix
  vgl_h_matrix_3d &
  set(unsigned int row_index, unsigned int col_index, T value)
  {
    t12_matrix_[row_index][col_index] = value;
    return *this;
  }

  //: Set to 4x4 row-stored matrix
  vgl_h_matrix_3d &
  set(const T * M);
  //: Set to given 4x4 matrix
  vgl_h_matrix_3d &
  set(const vnl_matrix_fixed<T, 4, 4> & M);

  // various affine transformations that set the corresponding parts of the matrix

  //: initialize the transformation to identity
  vgl_h_matrix_3d &
  set_identity();
  //: set H[0][3] = tx, H[1][3] = ty, and H[2][3] = tz, other elements unaltered
  vgl_h_matrix_3d &
  set_translation(T tx, T ty, T tz);
  //: compose the current transform with a uniform scaling transformation, S.
  // $S = \left[ \begin{array}{cccc}
  //                           s & 0 & 0 & 0 \\%
  //                           0 & s & 0 & 0 \\%
  //                           0 & 0 & s & 0 \\%
  //                           0 & 0 & 0 & 1
  // \end{array}\right]$                         , Ts = S*T.
  vgl_h_matrix_3d &
  set_scale(T scale);

  //: set the transform to a general affine transform matrix
  // $A = \left[ \begin{array}{ccc}
  //                           a00 & a01 & a02 & a03 \\%
  //                           a10 & a11 & a12 & a13 \\%
  //                           a20 & a21 & a22 & a23 \\%
  //                           0   & 0   & 0   & 1
  // \end{array}\right]$
  vgl_h_matrix_3d &
  set_affine(const vnl_matrix_fixed<T, 3, 4> & M34);

  //: Just the upper 3x3 part of the matrix is replaced by a rotation matrix.
  vgl_h_matrix_3d &
  set_rotation_matrix(const vnl_matrix_fixed<T, 3, 3> & R);
  //: Set to rotation about an axis
  //  Just the upper 3x3 part of the matrix is replaced by a rotation matrix.
  //  rotation angle theta is in radians
  vgl_h_matrix_3d &
  set_rotation_about_axis(const vnl_vector_fixed<T, 3> & axis, T theta);
  //: Set to roll, pitch and yaw specified rotation.
  // - roll is rotation about z
  // - pitch is rotation about y
  // - yaw is rotation about x
  //  Just the upper 3x3 part of the matrix is replaced by a rotation matrix.
  vgl_h_matrix_3d &
  set_rotation_roll_pitch_yaw(T yaw, T pitch, T roll);
  //: Set to rotation specified by Euler angles
  //  Just the upper 3x3 part of the matrix is replaced by a rotation matrix.
  vgl_h_matrix_3d &
  set_rotation_euler(T rz1, T ry, T rz2);

  //: set the transformation to a reflection about a plane
  void
  set_reflection_plane(const vgl_plane_3d<double> & p);

  bool
  is_rotation() const;
  bool
  is_identity() const;
  bool
  is_euclidean() const;
  bool
  is_affine() const;

  //: Compute transform to projective basis given five points, no 4 of which coplanar
  // Transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of points to the
  // canonical frame.  The points act as the projective basis for
  // the canonical coordinate system.  In the canonical frame the points
  // have coordinates:
  // $\begin{array}{cccc}
  //   p[0] & p[1] & p[2]  & p[3] & p[4] \\%
  //     1  &   0  &   0   &   0  &   1  \\%
  //     0  &   1  &   0   &   0  &   1  \\%
  //     0  &   0  &   1   &   0  &   1  \\%
  //     0  &   0  &   0   &   1  &   1
  // \end{array}$
  bool
  projective_basis(const std::vector<vgl_homg_point_3d<T>> & five_points);

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of planes to the canonical
  // frame.  The planes act as the dual projective basis for the canonical
  // coordinate system.  In the canonical frame the planes have equations:
  // x=0; y=0; z=0; w=0; x+y+z+w=0. (The latter plane is the plane at infinity.)
  bool
  projective_basis(const std::vector<vgl_homg_plane_3d<T>> & five_planes);

  // ---------- extract components as transformations ----------

  //: corresponds to rotation for Euclidean transformations
  vgl_h_matrix_3d<T>
  get_upper_3x3() const;
  vnl_matrix_fixed<T, 3, 3>
  get_upper_3x3_matrix() const;

  //: corresponds to translation for affine transformations
  vgl_homg_point_3d<T>
  get_translation() const;
  vnl_vector_fixed<T, 3>
  get_translation_vector() const;

  //: polar decomposition of the upper 3x3 matrix, M = S*R, where S is a symmetric matrix and R is an orthonormal matrix
  // useful for interpreting affine transformations
  void
  polar_decomposition(vnl_matrix_fixed<T, 3, 3> & S, vnl_matrix_fixed<T, 3, 3> & R) const;

  //: Load H from ASCII file.
  bool
  read(std::istream & s);
  //: Read H from file
  bool
  read(const char * filename);
};

//: Print H on std::ostream
template <class T>
std::ostream &
operator<<(std::ostream & s, const vgl_h_matrix_3d<T> & H);
//: Load H from ASCII file.
template <class T>
std::istream &
operator>>(std::istream & s, vgl_h_matrix_3d<T> & H)
{
  H.read(s);
  return s;
}

#define VGL_H_MATRIX_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_3d.hxx first"

#endif // vgl_h_matrix_3d_h_
