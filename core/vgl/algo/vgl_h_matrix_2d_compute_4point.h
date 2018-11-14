// This is core/vgl/algo/vgl_h_matrix_2d_compute_4point.h
#ifndef vgl_h_matrix_2d_compute_4point_h_
#define vgl_h_matrix_2d_compute_4point_h_
//--------------------------------------------------------------
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
//  Modifications
//   08-02-98 FSM obsoleted bool compute(vgl_h_matrix_2d<double>  *)
//   Mar 26, 2003 JLM Preparing to move up to vgl
//   Jun 23, 2003 Peter Vanroose - made compute_pl() etc. pure virtual
//   Jun 23, 2003 Peter Vanroose - added rough first impl. for compute_l()
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d_compute.h>

class vgl_h_matrix_2d_compute_4point : public vgl_h_matrix_2d_compute
{
 public:
  int minimum_number_of_correspondences() const override { return 4; }

 protected:
  //: compute from matched points

  bool compute_p(std::vector<vgl_homg_point_2d<double> > const& points1,
                 std::vector<vgl_homg_point_2d<double> > const& points2,
                 vgl_h_matrix_2d<double>& H) override;

  //:compute from matched lines

  bool compute_l(std::vector<vgl_homg_line_2d<double> > const& lines1,
                 std::vector<vgl_homg_line_2d<double> > const& lines2,
                 vgl_h_matrix_2d<double>& H) override;

  //:compute from matched lines with weight vector

  bool compute_l(std::vector<vgl_homg_line_2d<double> > const& lines1,
                 std::vector<vgl_homg_line_2d<double> > const& lines2,
                 std::vector<double> const& weights,
                 vgl_h_matrix_2d<double>& H) override;

  //:compute from matched points and lines

  bool compute_pl(std::vector<vgl_homg_point_2d<double> > const& points1,
                  std::vector<vgl_homg_point_2d<double> > const& points2,
                  std::vector<vgl_homg_line_2d<double> > const& lines1,
                  std::vector<vgl_homg_line_2d<double> > const& lines2,
                  vgl_h_matrix_2d<double>& H) override;
};

#endif // vgl_h_matrix_2d_compute_4point_h_
