// This is core/vgl/algo/vgl_h_matrix_1d.h
#ifndef vgl_h_matrix_1d_h_
#define vgl_h_matrix_1d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
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
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_1d.h>
#include <vcl_iosfwd.h>

template <class T>
class vgl_h_matrix_1d
{
  // Data Members--------------------------------------------------------------
 protected:
  vnl_matrix_fixed<T,2,2> t12_matrix_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_h_matrix_1d();
  vgl_h_matrix_1d(const vgl_h_matrix_1d<T>& M);
  // product of two vgl_h_matrix_1ds
  vgl_h_matrix_1d(const vgl_h_matrix_1d<T>&,const vgl_h_matrix_1d<T>&);
  vgl_h_matrix_1d(vnl_matrix_fixed<T,2,2> const& M);
  vgl_h_matrix_1d(const T* t_matrix);
  vgl_h_matrix_1d(vcl_istream& s);
 ~vgl_h_matrix_1d();

  // Operations----------------------------------------------------------------

  vgl_homg_point_1d<T > operator()(const vgl_homg_point_1d<T >& x1) const;
  vgl_homg_point_1d<T > preimage(const vgl_homg_point_1d<T >& x2) const;
  vgl_homg_point_1d<T > operator* (const vgl_homg_point_1d<T >& x1) const;
  // Data Access---------------------------------------------------------------

  T get (unsigned int row_index, unsigned int col_index) const;
  void get (T *t_matrix) const;
  void get (vnl_matrix<T >* t_matrix) const;
  const vnl_matrix_fixed<T,2,2 >& get_matrix () const { return t12_matrix_; }
  const vgl_h_matrix_1d get_inverse () const;

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
  bool projective_basis(vcl_vector<vgl_homg_point_1d<T> > const& three_points);

  void set_identity() { t12_matrix_.set_identity(); }
  void set(const T *t_matrix);
  void set(vnl_matrix_fixed<T,2,2> const& t_matrix);

  static vgl_h_matrix_1d<T > read(char const* filename);
  static vgl_h_matrix_1d<T > read(vcl_istream&);
};

template <class T> vcl_ostream& operator<<(vcl_ostream& s, const vgl_h_matrix_1d<T>& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, vgl_h_matrix_1d<T>& h);


#define VGL_H_MATRIX_1D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_1d.txx first"

#endif // vgl_h_matrix_1d_h_
