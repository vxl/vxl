#ifndef vgl_h_matrix_2d_compute_linear_h_
#define vgl_h_matrix_2d_compute_linear_h_
//:
// \file
// \brief contains class vgl_h_matrix_2d_compute_linear
//
// vgl_h_matrix_2d_compute_linear contains a linear method to calculate
// the plane projectivity which relates four 2D point (or plane)
// correspondences.
// The returned $H$ is such that
// $H  [p_1  p_2  p_3  p_4 ] \sim [p'_1  p'_2  p'_3  p'_4 ]$
// where the $p_i$ are the homogeneous points in the first view, and the
// $p'_i$ their images.
//
// \verbatim
//  Modifications
//   200598 FSM added checks for degenerate or coincident points.
//   230603 Peter Vanroose - made compute_pl() etc. pure virtual
//   240603 Peter Vanroose - added rough first implementation for compute_pl()
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d_compute.h>

class vgl_h_matrix_2d_compute_linear : public vgl_h_matrix_2d_compute
{
  bool allow_ideal_points_;
 protected:
  //: compute from matched points
  virtual
  bool compute_p(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                 vcl_vector<vgl_homg_point_2d<double> > const& points2,
                 vgl_h_matrix_2d<double>& H);

  //:compute from matched lines
  virtual
  bool compute_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                 vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                 vgl_h_matrix_2d<double>& H);

  //:compute from matched lines with weight vector
  virtual
  bool compute_l(vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                 vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                 vcl_vector<double> const& weights,
                 vgl_h_matrix_2d<double>& H);

  //:compute from matched points and lines
  virtual
  bool compute_pl(vcl_vector<vgl_homg_point_2d<double> > const& points1,
                  vcl_vector<vgl_homg_point_2d<double> > const& points2,
                  vcl_vector<vgl_homg_line_2d<double> > const& lines1,
                  vcl_vector<vgl_homg_line_2d<double> > const& lines2,
                  vgl_h_matrix_2d<double>& H);

  //:Assumes all corresponding points have equal weight
  bool solve_linear_problem(int equ_count,
                            vcl_vector<vgl_homg_point_2d<double> > const& p1,
                            vcl_vector<vgl_homg_point_2d<double> > const& p2,
                            vgl_h_matrix_2d<double>& H);

  //: for lines, the solution should be weighted by line length
  bool
  solve_weighted_least_squares(vcl_vector<vgl_homg_line_2d<double> > const& l1,
                               vcl_vector<vgl_homg_line_2d<double> > const& l2,
                               vcl_vector<double> const& w,
                               vgl_h_matrix_2d<double>& H);

 public:
  vgl_h_matrix_2d_compute_linear(bool allow_ideal_points = false);
  int minimum_number_of_correspondences() const { return 4; }
};

#endif // vgl_h_matrix_2d_compute_linear_h_
