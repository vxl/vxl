// This is brl/bbas/bvgl/bvgl_h_matrix_2d.h
#ifndef bvgl_h_matrix_2d_h_
#define bvgl_h_matrix_2d_h_
//:
//  \file
// \brief 3x3 plane-to-plane projectivity
//
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
//  Modifications
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//   22 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
// \endverbatim

#include <vcl_vector.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_conic.h>
#include <vcl_iosfwd.h>

//:
// A class to hold a plane-to-plane projective transformation matrix
// and to perform common operations using it e.g. transfer point.
template <class T>
class bvgl_h_matrix_2d
{
  // Data Members--------------------------------------------------------------
 protected:
  vnl_matrix_fixed<T,3,3> t12_matrix_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  bvgl_h_matrix_2d();
  bvgl_h_matrix_2d(const bvgl_h_matrix_2d<T>& M);
  bvgl_h_matrix_2d(vnl_matrix_fixed<T,3,3> const& M);
  bvgl_h_matrix_2d(const T* t_matrix);
  bvgl_h_matrix_2d(vcl_istream& s);
  bvgl_h_matrix_2d(char const* filename);

 ~bvgl_h_matrix_2d();

  // Operations----------------------------------------------------------------

  vgl_homg_point_2d<T> operator()(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_point_2d<T> operator*(vgl_homg_point_2d<T> const& p) const { return (*this)(p);}

  vgl_homg_line_2d<T> preimage(vgl_homg_line_2d<T> const& l) const;
  vgl_homg_line_2d<T> correlation(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_point_2d<T> correlation(vgl_homg_line_2d<T> const& l) const;

  //: assumed to be a point conic
  vgl_conic<T> operator() (vgl_conic<T> const& c) const;

  // these operations require taking an inverse
  vgl_homg_point_2d<T> preimage(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_line_2d<T> operator()(vgl_homg_line_2d<T> const& l) const;
  vgl_homg_line_2d<T> operator*(vgl_homg_line_2d<T> const& l) const { return (*this)(l);}

  //: Composition
  bvgl_h_matrix_2d operator*(bvgl_h_matrix_2d<T> const& h2) { return bvgl_h_matrix_2d(t12_matrix_ * h2.t12_matrix_); }

  // Data Access---------------------------------------------------------------

  T get(unsigned int row_index, unsigned int col_index) const;
  void get(T *t_matrix) const;
  void get(vnl_matrix<T>* t_matrix) const;
  const vnl_matrix_fixed<T,3,3>& get_matrix() const { return t12_matrix_; }
  const bvgl_h_matrix_2d get_inverse () const;

  void set_identity();
  void set(const T *t_matrix);
  void set(vnl_matrix_fixed<T,3,3> const& t_matrix);

  //: transformation to projective basis (canonical frame)
  bool projective_basis(vcl_vector<vgl_homg_point_2d<T> > const & four_points);

  bool read(vcl_istream& s);
  bool read(char const* filename);
};

template <class T> vcl_ostream& operator<<(vcl_ostream& s, const bvgl_h_matrix_2d<T>& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, bvgl_h_matrix_2d<T>& h);

#define BVGL_H_MATRIX_2D_INSTANTIATE(T) extern "please include bvgl/bvgl_h_matrix_2d.txx first"

#endif // bvgl_h_matrix_2d_h_
