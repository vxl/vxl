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
// \endverbatim

#include <vcl_vector.h>
#include <vcl_iosfwd.h>
#include <vnl/vnl_fwd.h> // for vnl_vector_fixed<T,3>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

//:
// A class to hold a 3-d projective transformation matrix
// and to perform common operations using it e.g. transform point.
template <class T>
class vgl_h_matrix_3d
{
 protected:
  vnl_matrix_fixed<T,4,4> t12_matrix_;
 public:
  vgl_h_matrix_3d();
  vgl_h_matrix_3d(const vgl_h_matrix_3d& M);
  vgl_h_matrix_3d(vnl_matrix_fixed<T,4,4> const& M);
  vgl_h_matrix_3d(vnl_matrix_fixed<T,3,3> const& M,
                  vnl_vector_fixed<T,3> const& m);
  vgl_h_matrix_3d(const T* t_matrix);
  vgl_h_matrix_3d(vcl_istream&);
 ~vgl_h_matrix_3d();

  // Operations----------------------------------------------------------------

  vgl_homg_point_3d<T> operator()(vgl_homg_point_3d<T> const& x) const;
  vgl_homg_point_3d<T> operator* (vgl_homg_point_3d<T> const& x) const {return (*this)(x);}

  vgl_homg_plane_3d<T> preimage(vgl_homg_plane_3d<T> const& p);

  //the following require forming an inverse
  vgl_homg_point_3d<T> preimage(vgl_homg_point_3d<T> const& x) const;
  vgl_homg_plane_3d<T> operator()(vgl_homg_plane_3d<T> const& x) const;

  bool read(vcl_istream&);

  // Data Access---------------------------------------------------------------

  T get (unsigned int row_index, unsigned int col_index) const;
  void get (T* t_matrix) const;
  void get (vnl_matrix_fixed<T, 4, 4>* t_matrix) const;
  const vnl_matrix_fixed<T,4,4>& get_matrix() const { return t12_matrix_; }
  vgl_h_matrix_3d get_inverse() const;

  void set(const T *t_matrix);
  void set(vnl_matrix_fixed<T,4,4> const& t_matrix);
  bool projective_basis(vcl_vector<vgl_homg_point_3d<T> > const & five_points);
  void set_identity();
  void set_translation(T tx, T ty, T tz);
  void set_rotation_about_axis(const vnl_vector_fixed<T,3>& axis, T angle);
  void set_rotation_roll_pitch_yaw(T yaw, T pitch, T roll);
  void set_rotation_euler(T rz1, T ry, T rz2);
};

// stream I/O
template <class T> vcl_ostream& operator<<(vcl_ostream& s, vgl_h_matrix_3d<T> const& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, vgl_h_matrix_3d<T>&       h);


#define VGL_H_MATRIX_3D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_3d.txx first"

#endif // vgl_h_matrix_3d_h_
