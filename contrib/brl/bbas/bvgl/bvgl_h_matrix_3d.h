// This is brl/bbas/bvgl/bvgl_h_matrix_3d.h
#ifndef bvgl_h_matrix_3d_h_
#define bvgl_h_matrix_3d_h_
//:
// \file
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

#include <vcl_iosfwd.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_plane_3d.h>

//:
// A class to hold a 3-d projective transformation matrix
// and to perform common operations using it e.g. transform point.
template <class T>
class bvgl_h_matrix_3d
{
 protected:
  vnl_matrix_fixed<T,4,4> t12_matrix_;
 public:
  bvgl_h_matrix_3d();
  bvgl_h_matrix_3d(const bvgl_h_matrix_3d& M);
  bvgl_h_matrix_3d(vnl_matrix_fixed<T, 4, 4> const& M);
  bvgl_h_matrix_3d(vnl_matrix_fixed<T,3, 3> const& M,
                   vnl_vector_fixed<T, 3> const& m);
  bvgl_h_matrix_3d(const T* t_matrix);
  bvgl_h_matrix_3d(vcl_istream&);
 ~bvgl_h_matrix_3d();

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
  bvgl_h_matrix_3d get_inverse() const;

  void set_identity();
  void set(const T *t_matrix);
  void set(vnl_matrix_fixed<T,4,4> const& t_matrix);
};

// stream I/O
template <class T> vcl_ostream& operator<<(vcl_ostream& s, bvgl_h_matrix_3d<T> const& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, bvgl_h_matrix_3d<T>&       h);


#define BVGL_H_MATRIX_3D_INSTANTIATE(T) extern "please include bvgl/bvgl_h_matrix_3d.txx first"

#endif // bvgl_h_matrix_3d_h_
