// This is core/vgl/algo/vgl_h_matrix_2d.h
#ifndef vgl_h_matrix_2d_h_
#define vgl_h_matrix_2d_h_
//:
// \file
// \brief 3x3 plane-to-plane projectivity
//
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
//  Modifications
//   16 Aug 2010 - Gamze Tunali - added is_identity()
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//   22 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//   24 Jun 2003 - Peter Vanroose - added projective_basis() from 4 lines
//   31 Jul 2010 - Peter Vanroose - made more similar to 1d and 3d variants
//   24 Oct 2010 - Peter Vanroose - mutators and setters now return *this
//   27 Oct 2010 - Peter Vanroose - moved Doxygen docs from .hxx to .h
// \endverbatim

#include <vector>
#include <iosfwd>
#include <vnl/vnl_fwd.h> // for vnl_vector_fixed<T,2>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_conic.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.
template <class T>
class vgl_h_matrix_2d
{
  // Data Members--------------------------------------------------------------
 protected:
  vnl_matrix_fixed<T,3,3> t12_matrix_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_h_matrix_2d() = default;
 ~vgl_h_matrix_2d() = default;
  //: Copy constructor
  vgl_h_matrix_2d(vgl_h_matrix_2d<T> const& M) : t12_matrix_(M.get_matrix()) {}
  //: Constructor from a 3x3 matrix, and implicit cast from vnl_matrix_fixed<T,3,3>
  vgl_h_matrix_2d(vnl_matrix_fixed<T,3,3> const& M) : t12_matrix_(M) {}
  //: Construct an affine vgl_h_matrix_2d from 2x2 M and 2x1 m.
  vgl_h_matrix_2d(vnl_matrix_fixed<T,2,2> const& M,
                  vnl_vector_fixed<T,2> const& m);
  //: Constructor from 3x3 C-array
  explicit vgl_h_matrix_2d(T const* M) : t12_matrix_(M) {}
  //: Constructor from istream
  explicit vgl_h_matrix_2d(std::istream& s);
  //: Constructor from file
  explicit vgl_h_matrix_2d(char const* filename);
  //: Constructor - calculate homography between two sets of 2D points (minimum 4)
  vgl_h_matrix_2d(std::vector<vgl_homg_point_2d<T> > const& points1,
                  std::vector<vgl_homg_point_2d<T> > const& points2);

  // Operations----------------------------------------------------------------

  //: Return the transformed point given by $q = {\tt H} p$
  vgl_homg_point_2d<T> operator()(vgl_homg_point_2d<T> const& p) const;
  //: Return the transformed point given by $q = {\tt H} p$
  vgl_homg_point_2d<T> operator*(vgl_homg_point_2d<T> const& p) const { return (*this)(p);}

  bool operator==(vgl_h_matrix_2d<T> const& M) const { return t12_matrix_ == M.get_matrix(); }

  //: Return the transformed line given by $m = {\tt H} l$
  vgl_homg_line_2d<T> preimage(vgl_homg_line_2d<T> const& l) const;
  vgl_homg_line_2d<T> correlation(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_point_2d<T> correlation(vgl_homg_line_2d<T> const& l) const;

  //: assumed to be a point conic
  vgl_conic<T> operator() (vgl_conic<T> const& C) const;

  // these operations require taking an inverse

  //: Return the transformed point given by $p = {\tt H}^{-1} q$
  vgl_homg_point_2d<T> preimage(vgl_homg_point_2d<T> const& q) const;
  //: Return the transformed line given by $m = {\tt H}^{-1} l$
  vgl_homg_line_2d<T> operator()(vgl_homg_line_2d<T> const& l) const;
  //: Return the transformed line given by $m = {\tt H}^{-1} l$
  vgl_homg_line_2d<T> operator*(vgl_homg_line_2d<T> const& l) const { return (*this)(l);}
  //: assumed to be a point conic
  vgl_conic<T> preimage(vgl_conic<T> const& C) const;

  //:composition (*this) * H
  vgl_h_matrix_2d<T> operator*(vgl_h_matrix_2d<T> const& H) const
  { return vgl_h_matrix_2d<T>(t12_matrix_ * H.t12_matrix_); }

  // Data Access---------------------------------------------------------------

  //: Return the 3x3 homography matrix
  vnl_matrix_fixed<T,3,3> const& get_matrix() const { return t12_matrix_; }
  //: Fill M with contents of the 3x3 homography matrix
  void get(vnl_matrix_fixed<T,3,3>* M) const;
  //:
  // \deprecated use the vnl_matrix_fixed variant instead
  void get(vnl_matrix<T>* M) const;
  //: Fill M with contents of the 3x3 homography matrix
  void get(T *M) const;
  //: Return an element from the 3x3 homography matrix
  T get(unsigned int row_index, unsigned int col_index) const;
  //: Return the inverse homography
  vgl_h_matrix_2d get_inverse() const;

  //: Set an element of the 3x3 homography matrix
  vgl_h_matrix_2d& set (unsigned int row_index, unsigned int col_index, T value)
  { t12_matrix_[row_index][col_index]=value; return *this; }

  //: Set to 3x3 row-stored matrix
  vgl_h_matrix_2d& set(T const* M);
  //: Set to given 3x3 matrix
  vgl_h_matrix_2d& set(vnl_matrix_fixed<T,3,3> const& M);

  // various affine transformations that set the corresponding parts of the matrix

  //: initialize the transformation to identity
  vgl_h_matrix_2d& set_identity();

  //: set H[0][2] = tx and H[1][2] = ty, other elements unaltered
  vgl_h_matrix_2d& set_translation(T tx, T ty);

  //: the upper 2x2 part of the matrix is replaced by a rotation matrix.
  //  rotation angle theta is in radians
  vgl_h_matrix_2d& set_rotation(T theta);

  //: compose the current transform with a uniform scaling transformation, S.
  // $S = \left[ \begin{array}{ccc}
  //                                s & 0 & 0 \\%
  //                                0 & s & 0 \\%
  //                                0 & 0 & 1
  // \end{array}\right]$                         , Ts = S*T.
  vgl_h_matrix_2d& set_scale(T scale);

  //: set the transform to a similarity mapping
  // Sim $ = \left[\begin{array}{ccc}
  //         \cos(\theta) & -\sin(\theta) & tx \\%
  //         \sin(\theta) &  \cos(\theta) & ty \\%
  //         0            &  0            & 1
  // \end{array}\right]$
  vgl_h_matrix_2d& set_similarity(T s, T theta, T tx, T ty);

  //: compose the transform with diagonal aspect ratio transform.
  // $A = \left[ \begin{array}{ccc}
  //                                 1 & 0 & 0 \\%
  //                                 0 & a & 0 \\%
  //                                 0 & 0 & 1
  // \end{array}\right]$                         , Ta = A*T.
  vgl_h_matrix_2d& set_aspect_ratio(T aspect_ratio);


  //: set the transform to a general affine transform matrix
  // $A = \left[ \begin{array}{ccc}
  //                           a00 & a01 & a02 \\%
  //                           a10 & a11 & a12 \\%
  //                           0   & 0   & 1
  // \end{array}\right]$
  vgl_h_matrix_2d& set_affine(vnl_matrix_fixed<T,2,3> const& M23);
  //:
  // \deprecated use the vnl_matrix_fixed variant instead
  vgl_h_matrix_2d& set_affine(vnl_matrix<T> const& M23);

  bool is_rotation() const;
  bool is_euclidean() const;
  bool is_identity() const;

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of points to the
  // canonical frame.  The points act as the projective basis for
  // the canonical coordinate system.  In the canonical frame the points
  // have coordinates:
  // $\begin{array}{cccc}
  //                 p[0] & p[1] & p[2] & p[3] \\%
  //                   1  &   0  &   0  &   1  \\%
  //                   0  &   1  &   0  &   1  \\%
  //                   0  &   0  &   1  &   1
  // \end{array}$
  bool projective_basis(std::vector<vgl_homg_point_2d<T> > const& four_points);

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of lines to the canonical
  // frame.  The lines act as the dual projective basis for the canonical
  // coordinate system.  In the canonical frame the lines have equations:
  // x=0; y=0; w=0; x+y+w=0.  (The third line is the line at infinity.)
  bool projective_basis(std::vector<vgl_homg_line_2d<T> > const& four_lines
                       );

  // ---------- extract components as transformations ----------

  //: corresponds to rotation for Euclidean transformations
  vgl_h_matrix_2d<T> get_upper_2x2() const;
  //: corresponds to rotation for Euclidean transformations
  vnl_matrix_fixed<T,2,2> get_upper_2x2_matrix() const;

  //: corresponds to translation for affine transformations
  vgl_homg_point_2d<T> get_translation() const;
  //: corresponds to translation for affine transformations
  vnl_vector_fixed<T,2> get_translation_vector() const;

  //: Read H from std::istream
  bool read(std::istream& s);
  //: Read H from file
  bool read(char const* filename);
};

//: Print H on std::ostream
template <class T> std::ostream& operator<<(std::ostream& s, vgl_h_matrix_2d<T> const& H);
//: Read H from std::istream
template <class T> std::istream& operator>>(std::istream& s, vgl_h_matrix_2d<T>&       H)
{ H.read(s); return s; }

#define VGL_H_MATRIX_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_2d.hxx first"

#endif // vgl_h_matrix_2d_h_
