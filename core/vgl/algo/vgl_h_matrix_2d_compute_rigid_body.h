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

  bool
  compute_p(const std::vector<vgl_homg_point_2d<double>> & points1,
            const std::vector<vgl_homg_point_2d<double>> & points2,
            vgl_h_matrix_2d<double> & H) override;

  //: compute from matched lines

  bool
  compute_l(const std::vector<vgl_homg_line_2d<double>> & lines1,
            const std::vector<vgl_homg_line_2d<double>> & lines2,
            vgl_h_matrix_2d<double> & H) override;

  //: compute from matched lines with weight vector (not implemented)

  bool
  compute_l(const std::vector<vgl_homg_line_2d<double>> & lines1,
            const std::vector<vgl_homg_line_2d<double>> & lines2,
            const std::vector<double> & weights,
            vgl_h_matrix_2d<double> & H) override;

  //: compute from matched points and lines

  bool
  compute_pl(const std::vector<vgl_homg_point_2d<double>> & points1,
             const std::vector<vgl_homg_point_2d<double>> & points2,
             const std::vector<vgl_homg_line_2d<double>> & lines1,
             const std::vector<vgl_homg_line_2d<double>> & lines2,
             vgl_h_matrix_2d<double> & H) override;
  //: Assumes all corresponding points have equal weight
  bool
  solve_rigid_body_problem(int equ_count,
                           const std::vector<vgl_homg_point_2d<double>> & p1,
                           const std::vector<vgl_homg_point_2d<double>> & p2,
                           vgl_h_matrix_2d<double> & H);


public:
  vgl_h_matrix_2d_compute_rigid_body();
  int
  minimum_number_of_correspondences() const override
  {
    return 2;
  }
};

#endif // vgl_h_matrix_2d_compute_rigid_body_h_
