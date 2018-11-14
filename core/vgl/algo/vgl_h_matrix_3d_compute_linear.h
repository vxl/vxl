#ifndef vgl_h_matrix_3d_compute_linear_h_
#define vgl_h_matrix_3d_compute_linear_h_
//:
// \file
// \brief contains class vgl_h_matrix_3d_compute_linear
// \author Ozge C. Ozcanli
// \date June 24, 2010
//
// vgl_h_matrix_3d_compute_linear uses the linear algorithm in point to point constructor of vgl_h_matrix_3d
// to calculate the homography which relates at least five 3D point correspondences.
// no four of them should be co-planar
//
// NOTE: the constructor of vgl_h_matrix_3d should not be used in itself as it does not standardize the point sets before
//       constructing the design matrix (which is the matrix that is decomposed using SVD)
//       the following class standardize the point sets and adjusts H accordingly
//
// The returned $H$ is such that
// $H  [p_1  p_2  p_3  p_4  p_5] \sim [p'_1  p'_2  p'_3  p'_4  p_5']$
// where the $p_i$ are the homogeneous points in 3D, and the
// $p'_i$ their images.
//
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vgl/algo/vgl_h_matrix_3d_compute.h>

class vgl_h_matrix_3d_compute_linear : public vgl_h_matrix_3d_compute
{
 protected:
  //: compute from matched points

  bool compute_p(std::vector<vgl_homg_point_3d<double> > const& points1,
                 std::vector<vgl_homg_point_3d<double> > const& points2,
                 vgl_h_matrix_3d<double>& H) override;

  //:Assumes all corresponding points have equal weight
  bool solve_linear_problem(std::vector<vgl_homg_point_3d<double> > const& p1,
                            std::vector<vgl_homg_point_3d<double> > const& p2,
                            vgl_h_matrix_3d<double>& H);

 public:
   vgl_h_matrix_3d_compute_linear() = default;
   int minimum_number_of_correspondences() const override { return 5; }
};

#endif // vgl_h_matrix_3d_compute_linear_h_
