// This is brl/bbas/vgl/algo/vgl_norm_trans_2d.h
#ifndef vgl_norm_trans_2d_h_
#define vgl_norm_trans_2d_h_
//:
//  \file
// \brief The similarity transform that normalizes a point set
//
// Algorithms to compute projective transformations require that
// the data be conditioned by insuring that the center of gravity
// of the point (line) set is at the origin and the standard deviation
// is isotropic and unity.
//
//\verbatim
// Created March 24, 2003 - J.L. Mundy
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_conic.h>
#include <vcl_iosfwd.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

template <class T>
class vgl_norm_trans_2d: public vgl_h_matrix_2d<T>
{
  // Data Members--------------------------------------------------------------

 public:

  // Constructors/Initializers/Destructors-------------------------------------

  vgl_norm_trans_2d();
  vgl_norm_trans_2d(const vgl_norm_trans_2d<T>& M);
  vgl_norm_trans_2d(vnl_matrix_fixed<T,3,3> const& M);
  vgl_norm_trans_2d(const T* t_matrix);
  vgl_norm_trans_2d(vcl_istream& s);
  vgl_norm_trans_2d(char const* filename);
 ~vgl_norm_trans_2d();

  // Operations----------------------------------------------------------------

 //: compute the normalizing transform
  bool compute_from_points(vcl_vector<vgl_homg_point_2d<T> > const& points);
  bool compute_from_lines(vcl_vector<vgl_homg_line_2d<T>  > const& lines);

  // Data Access---------------------------------------------------------------

 protected :
   //Utility functions

 static bool scale_xyroot2(vcl_vector<vgl_homg_point_2d<T> > const& in, 
                             T& radius);

 static void center_of_mass(vcl_vector<vgl_homg_point_2d<T> > const& points,
                            T& cx, T& cy);
  
};

#define BVGL_NORM_TRANS_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_norm_trans_2d.txx first"

#endif // vgl_norm_trans_2d_h_
