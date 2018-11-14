#ifndef vgl_h_matrix_2d_compute_rigid_body_h_
#define vgl_h_matrix_2d_compute_rigid_body_h_
//:
// \file
// \brief contains class vgl_h_matrix_2d_compute_rigid_body
// \author Joseph Mundy
// \date August 4, 2007
// vgl_h_matrix_2d_compute_rigid_body computes the rigid body transformation
// between points or lines
// \verbatim
//  Modifications
// none
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d_compute.h>

class vgl_h_matrix_2d_compute_rigid_body : public vgl_h_matrix_2d_compute
{
 protected:
  //: compute from matched points

  bool compute_p(std::vector<vgl_homg_point_2d<double> > const& points1,
                 std::vector<vgl_homg_point_2d<double> > const& points2,
                 vgl_h_matrix_2d<double>& H) override;

  //:compute from matched lines

  bool compute_l(std::vector<vgl_homg_line_2d<double> > const& lines1,
                 std::vector<vgl_homg_line_2d<double> > const& lines2,
                 vgl_h_matrix_2d<double>& H) override;

  //:compute from matched lines with weight vector (not implemented)

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
  //:Assumes all corresponding points have equal weight
  bool solve_rigid_body_problem(int equ_count,
                                std::vector<vgl_homg_point_2d<double> >const& p1,
                                std::vector<vgl_homg_point_2d<double> >const& p2,
                                vgl_h_matrix_2d<double>& H);


 public:
  vgl_h_matrix_2d_compute_rigid_body();
  int minimum_number_of_correspondences() const override { return 2; }
};

#endif // vgl_h_matrix_2d_compute_rigid_body_h_
