// This is core/vgl/algo/vgl_p_matrix.h
#ifndef vgl_p_matrix_h_
#define vgl_p_matrix_h_
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
//     110397 PVr Added operator==
//     221002 Peter Vanroose - added vgl_homg_point_2d interface
//     231002 Peter Vanroose - using fixed 3x4 matrices throughout
//     250503 J.L.M. converted to pure vgl infrastructure and templated
//            also made the interface a bit more consistent with 
//            plane projective transformations
//     270603 Peter Vanroose - moved doc from .txx to .h
//                           - implemented 3 NYI methods (get, set, set_rows)
// \endverbatim

#include <vcl_iosfwd.h>

#include <vnl/algo/vnl_algo_fwd.h> // for vnl_svd
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/algo/vgl_homg_operators_3d.h> //for p_matrix_ * vgl_homg_point_3d
#include <vgl/algo/vgl_h_matrix_3d.h>

template <class T>
class vgl_p_matrix 
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  //: Constructor. Set up a canonical P matrix.
  vgl_p_matrix();
  //: Construct by loading from vcl_istream.
  // \code
  //   vgl_p_matrix P(cin);
  // \endcode
  vgl_p_matrix(vcl_istream&);
  //: Construct from row-stored C-array of 12 elements
  vgl_p_matrix(const T *c_matrix);
  //: Construct from 3x4 matrix
  explicit vgl_p_matrix(vnl_matrix_fixed<T, 3, 4> const& P);
  //: Construct from 3x3 matrix A and vector a. P = [A a].
  vgl_p_matrix(const vnl_matrix<T>& A, const vnl_vector<T>& a);
  vgl_p_matrix(const vgl_p_matrix& P);
 ~vgl_p_matrix();

  //: Load from file.
  // Static method, so you can say
  // \code
  // vgl_p_matrix P = vgl_p_matrix::read("file.P");
  // \endcode
  static vgl_p_matrix read(const char* filename);
  //: Load from vcl_istream
  static vgl_p_matrix read(vcl_istream&);

  // Operations----------------------------------------------------------------

  //: Return the image point which is the projection of the specified 3D point X
  vgl_homg_point_2d<T>   operator()(vgl_homg_point_3d<T> const& X) const { return p_matrix_ * X; }
  //: Return the image point which is the projection of the specified 3D point X
  vgl_homg_point_2d<T>   operator*(vgl_homg_point_3d<T> const& X) const { return (*this)(X); }
  //: Return the image line which is the projection of the specified 3D line L
  vgl_homg_line_2d<T>    operator()(vgl_homg_line_3d_2_points<T> const& L) const;
  //: Return the image line which is the projection of the specified 3D line L
  vgl_homg_line_2d<T>   operator*(vgl_homg_line_3d_2_points<T> const& L) const { return (*this)(L);}
  //: Return the image linesegment which is the projection of the specified 3D linesegment L
  vgl_line_segment_2d<T> operator()(vgl_line_segment_3d<T> const& L) const;
  //: Return the image linesegment which is the projection of the specified 3D linesegment L
  vgl_line_segment_2d<T> operator*(vgl_line_segment_3d<T> const& L) const{return (*this)(L);}
  //: Return the 3D point $\vec X$ which is $\vec X = P^+ \vec x$.
  // Equivalently, the 3D point of smallest norm such that $P \vec X = \vec x$.
  // Uses svd().
  vgl_homg_point_3d<T> backproject_pseudoinverse(vgl_homg_point_2d<T> const& x) const;

  //: Return the 3D line which is the backprojection of the specified image point, x.
  // Uses svd().
  vgl_homg_line_3d_2_points<T>  backproject(vgl_homg_point_2d<T> const& x) const;
  //: Return the 3D plane which is the backprojection of the specified line l in the image
  vgl_homg_plane_3d<T> backproject(vgl_homg_line_2d<T> const& l) const;

  //: post-multiply this projection matrix with a 3-d projective transform
  vgl_p_matrix<T> postmultiply(vnl_matrix_fixed<T,4,4> const& H) const;

  //: pre-multiply this projection matrix with a 2-d projective transform
  vgl_p_matrix<T> premultiply(vnl_matrix_fixed<T,3,3> const& H) const;
  //: pre-multiply this projection matrix with a 2-d projective transform
  vgl_p_matrix<T> operator*(vnl_matrix_fixed<T, 3,3> const& C)const{return vgl_p_matrix(C * p_matrix_);}

  //: Compute the svd of this P and cache it, so that future operations that require it need not recompute it.
  vnl_svd<T>* svd() const; // mutable const
  //: Discredit the cached svd.
  //  This is necessary only in order to recover the space used by it if the vgl_p_matrix is not being deleted.
  void clear_svd() const;

  //: Return the 3D point representing the focal point of the camera.
  // Uses svd().
  vgl_homg_point_3d<T> get_focal() const;

  //: Return the 3D H-matrix s.t. P * H = [I 0].
  // If P = [A a], then H = [inv(A) -inv(A)*a; 0 0 0 1];
  vgl_h_matrix_3d<T> get_canonical_H() const;
  //: Return true iff P is [I 0].
  // Equality is assumed if the max abs diff is less than tol.
  bool is_canonical(T tol = 0) const;

  //: Return true if the 3D point X is behind the camera represented by this P.
  // This depends on the overall sign of the P matrix having been set correctly,
  // a la Hartley cheirality paper.
  bool is_behind_camera(vgl_homg_point_3d<T> const&);
  //: Change the overall sign of the P matrix.
  void flip_sign();
  //: Splendid hack that tries to detect if the P is an image-coords P or a normalized P.
  bool looks_conditioned();
  //: Scale P so determinant of first 3x3 is 1.
  void fix_cheirality();

  // Data Access---------------------------------------------------------------

  vgl_p_matrix& operator=(const vgl_p_matrix&);

  bool operator==(vgl_p_matrix const& p) const { return p_matrix_ == p.get_matrix(); }

  //: Return the 3x3 matrix and 3x1 column vector of P = [A a].
  void get(vnl_matrix<T>* A, vnl_vector<T>* a) const;
  //: Return the 3x3 matrix and 3x1 column vector of P = [A a].
  void get(vnl_matrix_fixed<T,3,3>* A, vnl_vector_fixed<T,3>* a) const;

  //: Return the rows of P = [a b c]'.
  void get_rows(vnl_vector<T>* a, vnl_vector<T>* b, vnl_vector<T>* c) const;
  //: Return the rows of P = [a b c]'.
  void get_rows(vnl_vector_fixed<T, 4>* a, vnl_vector_fixed<T, 4>* b, vnl_vector_fixed<T, 4>* c) const;
  //: Set P = [a b c]' from its rows a, b, c.
  void set_rows(const vnl_vector<T>& a, const vnl_vector<T>& b, const vnl_vector<T>& c);

  //: Return the element of the matrix at the specified index pair
  T get(unsigned int row_index, unsigned int col_index) const;
  //: Return the 3x4 projection matrix in the C-array, c_matrix
  void get(T *c_matrix) const;
  //: Return the 3x4 projection matrix in p_matrix
  void get(vnl_matrix<T>& p_matrix) const { p_matrix = p_matrix_; }
  //: Return the 3x4 projection matrix in p_matrix
  void get(vnl_matrix_fixed<T, 3, 4>& p_matrix) const { p_matrix = p_matrix_; }

  //: Set the 3x4 projective matrix with the matrix in the C-array, p_matrix
  void set(const T* p_matrix);
  //: Set the 3x4 projective matrix with the matrix in the C-array, p_matrix
  void set(const T p_matrix [3][4]);
  //: Set the internal matrix using the vnl_matrix<double> p_matrix.
  void set(const vnl_matrix<T>& p_matrix) { p_matrix_ = p_matrix; clear_svd(); }
  //: Set the internal matrix using the vnl_matrix<double> p_matrix.
  void set(vnl_matrix_fixed<T, 3, 4> const& p_matrix) { p_matrix_ = p_matrix; clear_svd(); }
  //: Set from 3x3 matrix and 3x1 column vector of P = [A a].
  void set(const vnl_matrix<T>& A, const vnl_vector<T>& a);

  const vnl_matrix_fixed<T, 3, 4>& get_matrix() const { return p_matrix_; }

  //: Set the camera to an identity projection. X->u, Y->v
  void set_identity();

  // Utility Methods-----------------------------------------------------------

  //: Load from file.
  // \code
  // P.read_ascii("file.P");
  // \endcode
  bool read_ascii(vcl_istream& f);

  // Data Members--------------------------------------------------------------
 protected:
  vnl_matrix_fixed<T, 3,4> p_matrix_;
  mutable vnl_svd<T>* svd_;
};

//: Postmultiply P-matrix P by 3D H-matrix H
template <class T>
vgl_p_matrix<T> operator*(const vgl_p_matrix<T>& P, const vgl_h_matrix_3d<T>& H);

//: Print p on an ostream
template <class T> vcl_ostream& operator<<(vcl_ostream& s, const vgl_p_matrix<T>& p);
//: Load p from an ascii istream
template <class T> vcl_istream& operator>>(vcl_istream& i, vgl_p_matrix<T>& p);

#define VGL_P_MATRIX_INSTANTIATE(T) extern "please include vgl/algo/vgl_p_matrix.txx first"

#endif // vgl_p_matrix_h_
