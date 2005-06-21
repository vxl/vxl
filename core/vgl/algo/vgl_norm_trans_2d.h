// This is core/vgl/algo/vgl_norm_trans_2d.h
#ifndef vgl_norm_trans_2d_h_
#define vgl_norm_trans_2d_h_
//:
// \file
// \brief The similarity transform that normalizes a point set
//
// Algorithms to compute projective transformations require that
// the data be conditioned by insuring that the center of gravity
// of the point (line) set is at the origin and the standard deviation
// is isotropic and unity. 
//
// The isotropic flag determines if a principal axis computation is 
// done to ansiotropically scale the points along the principal axes.
// If isotropic == true then points are scaled so that the average 
// point radius with respect to the center of mass is unity.  
// If isotropic = false then the points are rotated and radii scaled 
// according to the standard deviations along the principal axes.
//
// \verbatim
//  Modifications
//   Created March 24, 2003 - J.L. Mundy
//   May 15, 2003 - Added normalization computation for lines based on
//                  the point set defined by the intersection of each line
//                  with the perpendicular line from the origin.
//   Jun 23, 2003 - Peter Vanroose - added compute_from_points_and_lines()
//   Jun 17, 2005 - J.L. Mundy - added anisotropic scaling
// \endverbatim

#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vcl_iosfwd.h>
#include <vgl/algo/vgl_h_matrix_2d.h>

template <class T>
class vgl_norm_trans_2d: public vgl_h_matrix_2d<T>
{
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
  bool compute_from_points(vcl_vector<vgl_homg_point_2d<T> > const& points,
                           bool isotropic = true);
  bool compute_from_lines(vcl_vector<vgl_homg_line_2d<T>  > const& lines,
                          bool isotropic = true);
  bool 
    compute_from_points_and_lines(vcl_vector<vgl_homg_point_2d<T> > const& pts,
                                  vcl_vector<vgl_homg_line_2d<T> > const& lines
                                  , bool isotropic = true);

 protected :
  //Utility functions

  static bool scale_xyroot2(vcl_vector<vgl_homg_point_2d<T> > const& in,
                            T& radius);

  static void center_of_mass(vcl_vector<vgl_homg_point_2d<T> > const& points,
                             T& cx, T& cy);

  static bool scale_aniostropic(vcl_vector<vgl_homg_point_2d<T> > const& in,
                                T& sdx, T& sdy, T& c, T& s);
};

#define VGL_NORM_TRANS_2D_INSTANTIATE(T) extern "please include vgl/algo/vgl_norm_trans_2d.txx first"

#endif // vgl_norm_trans_2d_h_
