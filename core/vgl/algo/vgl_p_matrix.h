// This is brl/bmvl/vgl/algo/vgl_p_matrix.h
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

  vgl_p_matrix();
  vgl_p_matrix(vcl_istream&);
  vgl_p_matrix(const T *c_matrix);
  explicit vgl_p_matrix(vnl_matrix_fixed<T, 3, 4> const& P);
  vgl_p_matrix(const vnl_matrix<T>& A, const vnl_vector<T>& a);
  vgl_p_matrix(const vgl_p_matrix& P);
 ~vgl_p_matrix();

  static vgl_p_matrix read(const char* filename);
  static vgl_p_matrix read(vcl_istream&);

  // Operations----------------------------------------------------------------

  //: Return the image point which is the projection of the specified 3D point X
  vgl_homg_point_2d<T>   operator()(vgl_homg_point_3d<T> const& X) const { return p_matrix_ * X; }
  vgl_homg_point_2d<T>   operator*(vgl_homg_point_3d<T> const& X) const { return (*this)(X); }
  //: Return the image line which is the projection of the specified 3D line L
  vgl_homg_line_2d<T>    operator()(vgl_homg_line_3d_2_points<T> const& L) const;
  vgl_homg_line_2d<T>   operator*(vgl_homg_line_3d_2_points<T> const& L) const { return (*this)(L);}
  //: Return the image linesegment which is the projection of the specified 3D linesegment L
  vgl_line_segment_2d<T> operator()(vgl_line_segment_3d<T> const& L) const;
  vgl_line_segment_2d<T> operator*(vgl_line_segment_3d<T> const& L) const{return (*this)(L);}
  //: Return the 3D point $\vec X$ which is $\vec X = P^+ \vec x$.
  vgl_homg_point_3d<T> backproject_pseudoinverse(vgl_homg_point_2d<T> const& x) const;

  //: Return the 3D line which is the backprojection of the specified image point, x.
  vgl_homg_line_3d_2_points<T>  backproject(vgl_homg_point_2d<T> const& x) const;
  //: Return the 3D plane which is the backprojection of the specified line l in the image
  vgl_homg_plane_3d<T> backproject(vgl_homg_line_2d<T> const& l) const;

  //: post-multiply this projection matrix with a 3-d projective transform
  vgl_p_matrix<T> postmultiply(vnl_matrix_fixed<T,4,4> const& H) const;

  //: pre-multiply this projection matrix with a 2-d projective transform
  vgl_p_matrix<T> premultiply(vnl_matrix_fixed<T,3,3> const& H) const;

  vgl_p_matrix<T> operator*(vnl_matrix_fixed<T, 3,3> const& C){return vgl_p_matrix(C * p_matrix_);}

  vnl_svd<T>* svd() const; // mutable const
  void clear_svd() const;
  vgl_homg_point_3d<T> get_focal() const;

  vgl_h_matrix_3d<T> get_canonical_H() const;
  bool is_canonical(T tol = 0) const;

  bool is_behind_camera(vgl_homg_point_3d<T> const&);
  void flip_sign();
  bool looks_conditioned();
  void fix_cheirality();

  // Data Access---------------------------------------------------------------

  vgl_p_matrix& operator=(const vgl_p_matrix&);

  bool operator==(vgl_p_matrix const& p) const { return p_matrix_ == p.get_matrix(); }

  void get(vnl_matrix<T>* A, vnl_vector<T>* a) const;
  void get(vnl_matrix_fixed<T,3,3>* A, vnl_vector_fixed<T,3>* a) const;
  void set(const vnl_matrix<T>& A, const vnl_vector<T>& a);

  void get_rows(vnl_vector<T>*, vnl_vector<T>*, vnl_vector<T>*) const;
  void get_rows(vnl_vector_fixed<T, 4>*, vnl_vector_fixed<T, 4>*, vnl_vector_fixed<T, 4>*) const;
  void set_rows(const vnl_vector<T>&, const vnl_vector<T>&, const vnl_vector<T>&);

  T get(unsigned int row_index, unsigned int col_index) const;
  void get(T *c_matrix) const;
  void get(vnl_matrix<T>& p_matrix) const;
  void get(vnl_matrix_fixed<T, 3, 4>& p_matrix) const;

  void set(const T* p_matrix);
  void set(const T p_matrix [3][4]);
  void set(const vnl_matrix<T>& p_matrix);
  void set(vnl_matrix_fixed<T, 3, 4> const& p_matrix);
  const vnl_matrix_fixed<T, 3, 4>& get_matrix() const { return p_matrix_; }
  void set_identity();

  // Utility Methods-----------------------------------------------------------
  bool read_ascii(vcl_istream& f);

  // Data Members--------------------------------------------------------------
 protected:
  vnl_matrix_fixed<T, 3,4> p_matrix_;
  mutable vnl_svd<T>* svd_;
};



template <class T> vcl_ostream& operator<<(vcl_ostream& s, const vgl_p_matrix<T>& p);
template <class T> vcl_istream& operator>>(vcl_istream& i, vgl_p_matrix<T>& p);

#define MVGL_P_MATRIX_INSTANTIATE(T) extern "please include vgl/algo/vgl_p_matrix.txx first"

#endif // vgl_p_matrix_h_
