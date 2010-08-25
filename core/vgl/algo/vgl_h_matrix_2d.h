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
//   16 Aug 2010 - Gamze Tunali - added is_identity(), b_read(), b_write(),vsl_b_write()
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//   22 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//   24 Jun 2003 - Peter Vanroose - added projective_basis() from 4 lines
//   31 Jul 2010 - Peter Vanroose - made more similar to 1d and 3d variants
// \endverbatim

#include <vcl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_conic.h>
#include <vcl_iosfwd.h>
#include <vsl/vsl_binary_io.h>

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

  vgl_h_matrix_2d() {}
 ~vgl_h_matrix_2d() {}
  vgl_h_matrix_2d(vgl_h_matrix_2d<T> const& M);
  vgl_h_matrix_2d(vnl_matrix_fixed<T,3,3> const& M);
  vgl_h_matrix_2d(vnl_matrix_fixed<T,2,2> const& M,
                  vnl_vector_fixed<T,2> const& m);
  explicit vgl_h_matrix_2d(T const* t_matrix);
  explicit vgl_h_matrix_2d(vcl_istream& s);
  explicit vgl_h_matrix_2d(char const* filename);
  vgl_h_matrix_2d(vcl_vector<vgl_homg_point_2d<T> > const& points1,
                  vcl_vector<vgl_homg_point_2d<T> > const& points2);

  // Operations----------------------------------------------------------------

  vgl_homg_point_2d<T> operator()(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_point_2d<T> operator*(vgl_homg_point_2d<T> const& p) const { return (*this)(p);}
  bool operator==(vgl_h_matrix_2d<T> const& M) const { return t12_matrix_ == M.get_matrix(); }

  vgl_homg_line_2d<T> preimage(vgl_homg_line_2d<T> const& l) const;
  vgl_homg_line_2d<T> correlation(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_point_2d<T> correlation(vgl_homg_line_2d<T> const& l) const;

  //: assumed to be a point conic
  vgl_conic<T> operator() (vgl_conic<T> const& C) const;

  // these operations require taking an inverse
  vgl_homg_point_2d<T> preimage(vgl_homg_point_2d<T> const& p) const;
  vgl_conic<T> preimage(vgl_conic<T> const& C) const;
  vgl_homg_line_2d<T> operator()(vgl_homg_line_2d<T> const& l) const;
  vgl_homg_line_2d<T> operator*(vgl_homg_line_2d<T> const& l) const { return (*this)(l);}

  //: Composition
  vgl_h_matrix_2d<T> operator*(vgl_h_matrix_2d<T> const& H) const
  { return vgl_h_matrix_2d<T>(t12_matrix_ * H.t12_matrix_); }

  // Data Access---------------------------------------------------------------

  T get(unsigned int row_index, unsigned int col_index) const;
  void get(T *t_matrix) const;
  void get(vnl_matrix_fixed<T,3,3>* t_matrix) const;
  void get(vnl_matrix<T>* t_matrix) const;
  vnl_matrix_fixed<T,3,3> const& get_matrix() const { return t12_matrix_; }
  vgl_h_matrix_2d get_inverse() const;

  void set (unsigned int row_index, unsigned int col_index, const T value)
  { t12_matrix_[row_index][col_index]=value; }

  void set(const T *t_matrix);
  void set(vnl_matrix_fixed<T,3,3> const& t_matrix);

  // various affine transformations that set the corresponding parts of the matrix

  //:initialize the transformation to identity
  void set_identity();

  //: set T[0][2] = tx and T[1][2] = ty, other elements unaltered
  void set_translation(const T tx, const T ty);

  //: the upper 2x2 part of the matrix is replaced by a rotation matrix.
  // theta is in radians
  void set_rotation(const T theta);

  //: compose the current transform with a scaling transform, S.
  // $S = \left[ \begin{array}{ccc}
  //                                s & 0 & 0 \\%
  //                                0 & s & 0 \\%
  //                                0 & 0 & 1
  // \end{array}\right]$                         , Ts = S*T.
  void set_scale(const T scale);

  //: set the transform to a similarity mapping
  // Sim $ = \left[\begin{array}{ccc}
  //         \cos(\theta) & -\sin(\theta) & tx \\%
  //         \sin(\theta) &  \cos(\theta) & ty \\%
  //         0            &  0            & 1
  // \end{array}\right]$
  void set_similarity(const T s, const T theta, const T tx, const T ty);

  //: compose the transform with diagonal aspect transform.
  // $A = \left[ \begin{array}{ccc}
  //                                 1 & 0 & 0 \\%
  //                                 0 & a & 0 \\%
  //                                 0 & 0 & 1
  // \end{array}\right]$                         , Ta = A*T.
  void set_aspect_ratio(const T aspect_ratio);


  //: set the transform to a general affine transform matrix
  // $A = \left[ \begin{array}{ccc}
  //                           a00 & a01 & a02 \\%
  //                           a10 & a11 & a12 \\%
  //                           0   & 0   & 1
  // \end{array}\right]$
  void set_affine(const vnl_matrix<T>& M23);

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
  bool projective_basis(vcl_vector<vgl_homg_point_2d<T> > const& four_points);

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of lines to the canonical
  // frame.  The lines act as the dual projective basis for the canonical
  // coordinate system.  In the canonical frame the lines have equations:
  // x=0; y=0; w=0; x+y+w=0.  (The third line is the line at infinity.)
  bool projective_basis(vcl_vector<vgl_homg_line_2d<T> > const& four_lines
#ifdef VCL_VC_6
                       , int dummy=0 // parameter to help useless compiler disambiguate different functions
#endif
                       );

  // ---------- extract components as transformations ----------

  //: corresponds to rotation for Euclidean transformations
  vgl_h_matrix_2d<T> get_upper_2x2() const;
  vnl_matrix_fixed<T, 2,2> get_upper_2x2_matrix() const;

  //: corresponds to translation for affine transformations
  vgl_homg_point_2d<T> get_translation() const;
  vnl_vector_fixed<T, 2> get_translation_vector() const;

  bool read(vcl_istream& s);
  bool read(char const* filename);

  void b_write(vsl_b_ostream& bfs) const;
  void b_read(vsl_b_istream& bfs);
  short version_no() const { return 1; }
};

template <class T> vcl_ostream& operator<<(vcl_ostream& s, vgl_h_matrix_2d<T> const& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, vgl_h_matrix_2d<T>&       h);

template <class T> void vsl_b_write(vsl_b_ostream &os, vgl_h_matrix_2d<T> const* t)
{
  if (t==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    t->b_write(os);
  }
}


#define VGL_H_MATRIX_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_2d.txx first"

#endif // vgl_h_matrix_2d_h_
