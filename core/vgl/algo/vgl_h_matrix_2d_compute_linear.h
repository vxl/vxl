#ifndef _vgl_h_matrix_2d_compute_linear_h
#define _vgl_h_matrix_2d_compute_linear_h
//:
// \file
//
// vgl_h_matrix_2d_compute_linear contains a linear method to calculate
// the plane projectivity which relates four 2D point correspondences.
// The returned $H$ is such that
// $H ~ [p_1 ~ p_2 ~ p_3 ~ p_4 ] \sim [p'_1 ~ p'_2 ~ p'_3 ~ p'_4 ]$
// where the $p_i$ are the homogeneous points in the first view, and the
// $p'_i$ their images.
//
// \verbatim
// Modifications:
//     200598 FSM added checks for degenerate or coincident points.
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d_compute.h>
class vgl_h_matrix_2d_compute_linear : public vgl_h_matrix_2d_compute
{
  bool allow_ideal_points_;
 protected:
  virtual 
    //:compute from matched points implementation only for now
bool compute_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                vcl_vector<vgl_homg_point_2d<double> > const& points2, 
                vgl_h_matrix_2d<double>& H);

 public:
  vgl_h_matrix_2d_compute_linear(bool allow_ideal_points = false);
  int minimum_number_of_correspondences() const { return 4; }
};

#endif // _vgl_h_matrix_2d_compute_linear_h
