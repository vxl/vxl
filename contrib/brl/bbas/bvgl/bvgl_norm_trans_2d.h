// This is brl/bbas/bvgl/bvgl_norm_trans_2d.h
#ifndef bvgl_norm_trans_2d_h_
#define bvgl_norm_trans_2d_h_
//:
// \file
// \brief The similarity transform that normalizes a point set
//
// Algorithms to compute projective transformations require that
// the data be conditioned by insuring that the center of gravity
// of the point (line) set is at the origin and the standard deviation
// is isotropic and unity.
//
// \verbatim
//  Modifications
//   Created March 24, 2003 - J.L. Mundy
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vcl_iosfwd.h>
#include <bvgl/bvgl_h_matrix_2d.h>

template <class T>
class bvgl_norm_trans_2d: public bvgl_h_matrix_2d<T>
{
 public:

  // Constructors/Initializers/Destructors-------------------------------------

  bvgl_norm_trans_2d();
  bvgl_norm_trans_2d(const bvgl_norm_trans_2d<T>& M);
  bvgl_norm_trans_2d(vnl_matrix_fixed<T,3,3> const& M);
  bvgl_norm_trans_2d(const T* t_matrix);
  bvgl_norm_trans_2d(vcl_istream& s);
  bvgl_norm_trans_2d(char const* filename);
 ~bvgl_norm_trans_2d();

  // Operations----------------------------------------------------------------

  //: compute the normalizing transform
  bool compute_from_points(vcl_vector<vgl_homg_point_2d<T> > const& points);
  bool compute_from_lines(vcl_vector<vgl_homg_line_2d<T>  > const& lines);

 protected :
  //Utility functions

  static bool scale_xyroot2(vcl_vector<vgl_homg_point_2d<T> > const& in, 
                            T& radius);

  static void center_of_mass(vcl_vector<vgl_homg_point_2d<T> > const& points,
                             T& cx, T& cy);
};

#define BVGL_NORM_TRANS_2D_INSTANTIATE(T) extern "please include bvgl/bvgl_norm_trans_2d.txx first"

#endif // bvgl_norm_trans_2d_h_
