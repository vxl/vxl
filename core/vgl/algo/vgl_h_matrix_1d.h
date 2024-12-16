// This is core/vgl/algo/vgl_h_matrix_1d.h
#ifndef vgl_h_matrix_1d_h_
#define vgl_h_matrix_1d_h_
//:
// \file
// \brief 2x2 line-to-line projectivity
//
// A class to hold a line-to-line projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//   22 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//   13 Jun 2004 - Peter Vanroose - added set_identity() and projective_basis()
//   31 Jul 2010 - Peter Vanroose - made more similar to 2d and 3d variants
//   24 Oct 2010 - Peter Vanroose - mutators and setters now return *this
//   27 Oct 2010 - Peter Vanroose - moved Doxygen docs from .hxx to .h
//   26 Jul 2011 - Peter Vanroose - added is_identity(),is_rotation(),is_euclidean(),
//                                  correlation(),operator*(h_matrix),set(row,col,val),
//                                  set_translation(),set_affine(), cf vgl_h_matrix_2d
// \endverbatim

#include <vector>
#include <iosfwd>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_1d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// A class to hold a line-to-line projective transformation matrix
// and to perform common operations using it e.g. transfer point.
template <class T>
class vgl_h_matrix_1d
{
  // Data Members--------------------------------------------------------------
protected:
  vnl_matrix_fixed<T, 2, 2> t12_matrix_;

public:
  // Constructors/Initializers/Destructors-------------------------------------

  vgl_h_matrix_1d() = default;
  ~vgl_h_matrix_1d() = default;

  //: Copy constructor
  vgl_h_matrix_1d(const vgl_h_matrix_1d<T> & M)
    : t12_matrix_(M.get_matrix())
  {}

  //: Constructor from a 2x2 matrix, and implicit cast from vnl_matrix_fixed<T,2,2>
  vgl_h_matrix_1d(const vnl_matrix_fixed<T, 2, 2> & M)
    : t12_matrix_(M)
  {}

  //: Constructor from 2x2 C-array
  explicit vgl_h_matrix_1d(const T * M)
    : t12_matrix_(M)
  {}
  //: Constructor from istream
  explicit vgl_h_matrix_1d(std::istream & s);
  //: Constructor from file
  explicit vgl_h_matrix_1d(const char * filename);
  //: Constructor - calculate homography between two sets of 1D points (minimum 3)
  vgl_h_matrix_1d(const std::vector<vgl_homg_point_1d<T>> & points1, const std::vector<vgl_homg_point_1d<T>> & points2);

  // Operations----------------------------------------------------------------

  //: Return the transformed point given by $q = {\tt H} p$
  vgl_homg_point_1d<T>
  operator()(const vgl_homg_point_1d<T> & p) const;
  //: Return the transformed point given by $q = {\tt H} p$
  vgl_homg_point_1d<T>
  operator*(const vgl_homg_point_1d<T> & p) const
  {
    return (*this)(p);
  }
  vgl_homg_point_1d<T>
  correlation(const vgl_homg_point_1d<T> & p) const
  {
    return (*this)(p);
  }

  //: Return the transformed point given by $p = {\tt H}^{-1} q$
  vgl_homg_point_1d<T>
  preimage(const vgl_homg_point_1d<T> & q) const;

  bool
  operator==(const vgl_h_matrix_1d<T> & M) const
  {
    return t12_matrix_ == M.get_matrix();
  }

  //: product of two vgl_h_matrix_1ds
  vgl_h_matrix_1d<T>
  operator*(const vgl_h_matrix_1d<T> & H) const
  {
    return vgl_h_matrix_1d<T>(t12_matrix_ * H.t12_matrix_);
  }

  // Data Access---------------------------------------------------------------

  //: Return the 2x2 homography matrix
  const vnl_matrix_fixed<T, 2, 2> &
  get_matrix() const
  {
    return t12_matrix_;
  }
  //: Fill M with contents of the 2x2 homography matrix
  void
  get(vnl_matrix_fixed<T, 2, 2> * M) const;

  //: Fill M with contents of the 2x2 homography matrix
  void
  get(T * M) const;
  //: Return an element from the 2x2 homography matrix
  T
  get(unsigned int row_index, unsigned int col_index) const;
  //: Return the inverse homography
  vgl_h_matrix_1d
  get_inverse() const;

  bool
  is_rotation() const;
  bool
  is_euclidean() const;
  bool
  is_identity() const;

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of points to the
  // canonical frame.  The points act as the projective basis for
  // the canonical coordinate system.  In the canonical frame the points
  // have coordinates:
  // \verbatim
  //   p[0]p[1]p[2]
  //     1   0   1
  //     0   1   1
  // \endverbatim
  bool
  projective_basis(const std::vector<vgl_homg_point_1d<T>> & three_points);

  //: initialize the transformation to identity
  vgl_h_matrix_1d &
  set_identity();
  //: Set to 2x2 row-stored matrix
  vgl_h_matrix_1d &
  set(const T * M);
  //: Set to given 2x2 matrix
  vgl_h_matrix_1d &
  set(const vnl_matrix_fixed<T, 2, 2> & M);

  //: Set an element of the 2x2 homography matrix
  vgl_h_matrix_1d &
  set(unsigned int row_index, unsigned int col_index, T value)
  {
    t12_matrix_[row_index][col_index] = value;
    return *this;
  }

  //: compose the current transform with a uniform scaling transformation, S.
  // $S = \left[ \begin{array}{cc}
  //                                s & 0 \\%
  //                                0 & 1
  // \end{array}\right]$                         , Ts = S*T.
  vgl_h_matrix_1d &
  set_scale(T scale);

  //: set H[0][1] = tx, other elements unaltered
  vgl_h_matrix_1d &
  set_translation(T tx);

  //: set the transform to a general affine transform matrix
  // $A = \left[ \begin{array}{ccc}
  //                           a00 & a01 \\%
  //                           0   & 1
  // \end{array}\right]$
  vgl_h_matrix_1d &
  set_affine(const vnl_matrix_fixed<T, 1, 2> & M12);

  //: Read H from file
  bool
  read(const char * filename);
  //: Read H from std::istream
  bool
  read(std::istream & s);
};

//: Print H on std::ostream
template <class T>
std::ostream &
operator<<(std::ostream & s, const vgl_h_matrix_1d<T> & H);
//: Read H from std::istream
template <class T>
std::istream &
operator>>(std::istream & s, vgl_h_matrix_1d<T> & H)
{
  H.read(s);
  return s;
}


#define VGL_H_MATRIX_1D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_1d.hxx first"

#endif // vgl_h_matrix_1d_h_
