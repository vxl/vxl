// This is brl/bbas/bvgl/bvgl_h_matrix_1d.h
#ifndef bvgl_h_matrix_1d_h_
#define bvgl_h_matrix_1d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif

//:
// \file
//
// A class to hold a line-to-line projective transformation matrix
// and to perform common operations using it e.g. transfer point.
//
// \verbatim
// Modifications
//    22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//    23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//    22 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_iosfwd.h>

template <class T>
class bvgl_h_matrix_1d
{
  // Data Members--------------------------------------------------------------
  vnl_matrix_fixed<T,2,2> t12_matrix_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  bvgl_h_matrix_1d();
  bvgl_h_matrix_1d(const bvgl_h_matrix_1d<T>& M);
  // product of two bvgl_h_matrix_1ds
  bvgl_h_matrix_1d(const bvgl_h_matrix_1d<T>&,const bvgl_h_matrix_1d<T>&);
  bvgl_h_matrix_1d(vnl_matrix_fixed<T,2,2> const& M);
  bvgl_h_matrix_1d(const T* t_matrix);
  bvgl_h_matrix_1d(vcl_istream& s);
 ~bvgl_h_matrix_1d();

  // Operations----------------------------------------------------------------

  vgl_homg_point_1d<T > operator()(const vgl_homg_point_1d<T >& x1) const;
  vgl_homg_point_1d<T > preimage(const vgl_homg_point_1d<T >& x2) const;
  vgl_homg_point_1d<T > operator* (const vgl_homg_point_1d<T >& x1) const;
  // Data Access---------------------------------------------------------------

  T get (unsigned int row_index, unsigned int col_index) const;
  void get (T *t_matrix) const;
  void get (vnl_matrix<T >* t_matrix) const;
  const vnl_matrix_fixed<T,2,2 >& get_matrix () const { return t12_matrix_; }
  const bvgl_h_matrix_1d get_inverse () const;

  void set (const T *t_matrix);
  void set (vnl_matrix_fixed<T,2,2> const& t_matrix);

  static bvgl_h_matrix_1d<T > read(char const* filename);
  static bvgl_h_matrix_1d<T > read(vcl_istream&);
};

template <class T> vcl_ostream& operator<<(vcl_ostream& s, const bvgl_h_matrix_1d<T>& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, bvgl_h_matrix_1d<T>& h);


#define BVGL_H_MATRIX_1D_INSTANTIATE(T) extern "please include bvgl/bvgl_h_matrix_1d.txx first"

#endif // bvgl_h_matrix_1d_h_
