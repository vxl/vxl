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
//   22 Oct 2002 - Peter Vanroose - added vgl_homg_point_2d interface
//   23 Oct 2002 - Peter Vanroose - using fixed 3x3 matrices throughout
//   22 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//   24 Jun 2003 - Peter Vanroose - added projective_basis() from 4 lines
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
class vgl_h_matrix_2d
{
  // Data Members--------------------------------------------------------------
 protected:
  vnl_matrix_fixed<T,3,3> t12_matrix_;

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_h_matrix_2d();
  vgl_h_matrix_2d(const vgl_h_matrix_2d<T>& M);
  vgl_h_matrix_2d(vnl_matrix_fixed<T,3,3> const& M);
  vgl_h_matrix_2d(const T* t_matrix);
  vgl_h_matrix_2d(vcl_istream& s);
  vgl_h_matrix_2d(char const* filename);

 ~vgl_h_matrix_2d();

  // Operations----------------------------------------------------------------

  vgl_homg_point_2d<T> operator()(vgl_homg_point_2d<T> const& p) const;
  vgl_homg_point_2d<T> operator*(vgl_homg_point_2d<T> const& p) const { return (*this)(p);}

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
  vgl_h_matrix_2d operator*(const vgl_h_matrix_2d& h2) const { return vgl_h_matrix_2d<T>(t12_matrix_ * h2.t12_matrix_); }

  // Data Access---------------------------------------------------------------

  T get(unsigned int row_index, unsigned int col_index) const;
  void get(T *t_matrix) const;
  void get(vnl_matrix<T>* t_matrix) const;
  const vnl_matrix_fixed<T,3,3>& get_matrix() const { return t12_matrix_; }
  const vgl_h_matrix_2d get_inverse () const;

  void set_identity();
  void set(const T *t_matrix);
  void set(vnl_matrix_fixed<T,3,3> const& t_matrix);

  //:various transformations that set the corresponding parts of the matrix
  void set_translation(const T tx, const T ty);
  //: theta is in radians, the upper 2x2 part of the matrix is replaced
  void set_rotation(const T theta);
  //: transform is composed with a scaling transform (only upper 2x2 affected)
  void set_scale(const T scale);

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of points to the
  // canonical frame.  The points act as the projective basis for
  // the canonical coordinate system.  In the canonical frame the points
  // have coordinates:
  // \verbatim
  //   p[0]p[1]p[2]p[3]
  //     1   0   0   1
  //     0   1   0   1
  //     0   0   1   1
  // \endverbatim
  bool projective_basis(vcl_vector<vgl_homg_point_2d<T> > const& four_points);

  //: transformation to projective basis (canonical frame)
  // Compute the homography that takes the input set of lines to the canonical
  // frame.  The lines act as the dual projective basis for the canonical
  // coordinate system.  In the canonical frame the lines have equations:
  // x=0; y=0; w=0; x+y+w=0.  (The third line is the line at infinity.)
  bool projective_basis(vcl_vector<vgl_homg_line_2d<T> > const& four_lines
#ifdef VCL_VC60
    , int dummy=0 // parameter to help useless compiler disambiguate different functions
#endif
    );

  bool read(vcl_istream& s);
  bool read(char const* filename);
};

template <class T> vcl_ostream& operator<<(vcl_ostream& s, const vgl_h_matrix_2d<T>& h);
template <class T> vcl_istream& operator>>(vcl_istream& s, vgl_h_matrix_2d<T>& h);

#define VGL_H_MATRIX_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_h_matrix_2d.txx first"

#endif // vgl_h_matrix_2d_h_
