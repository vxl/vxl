#ifndef vgl_h_matrix_3d_compute_affine_h_
#define vgl_h_matrix_3d_compute_affine_h_
//:
// \file
// \brief contains class vgl_h_matrix_3d_compute_affine
// \author J.L. Mundy
// \date Sept. 7, 2015
//
// vgl_h_matrix_3d_compute_affine requires at least four 3-d point correspondences
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vgl/algo/vgl_h_matrix_3d_compute.h>
#include <vnl/vnl_matrix.h>
class vgl_h_matrix_3d_compute_affine : public vgl_h_matrix_3d_compute
{
protected:
  //: compute from matched points

  bool
  compute_p(const std::vector<vgl_homg_point_3d<double>> & points1,
            const std::vector<vgl_homg_point_3d<double>> & points2,
            vgl_h_matrix_3d<double> & H) override;

  //: Assumes all corresponding points have equal weight
  bool
  solve_linear_problem(const std::vector<vgl_homg_point_3d<double>> & p1,
                       const std::vector<vgl_homg_point_3d<double>> & p2,
                       vnl_matrix<double> & M);


public:
  vgl_h_matrix_3d_compute_affine() = default;
  int
  minimum_number_of_correspondences() const override
  {
    return 4;
  }
};

#endif // vgl_h_matrix_3d_compute_affine_h_
