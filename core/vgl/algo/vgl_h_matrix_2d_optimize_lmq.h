#ifndef vgl_h_matrix_2d_optimize_lmq_h_
#define vgl_h_matrix_2d_optimize_lmq_h_
//:
// \file
// \author  J.L. Mundy Jan 05, 2005
// \brief contains class vgl_h_matrix_2d_optimize_lmq
//
// vgl_h_matrix_2d_optimize_lmq uses the Levenberg-Marquardt algorithm
// to refine an initial value for vgl_h_matrix_2d, given a set of corresponding
// points or lines.  The number of points/lines must be greater than four,
// since for four or fewer points/lines the projective mapping is exact.
// \verbatim
//  Modifications
//   none
// \endverbatim

#include <vgl/algo/vgl_h_matrix_2d_optimize.h>

class vgl_h_matrix_2d_optimize_lmq : public vgl_h_matrix_2d_optimize
{
public:
  //: Constructor from initial homography to be optimized
  vgl_h_matrix_2d_optimize_lmq(const vgl_h_matrix_2d<double> & initial_h);

  int
  minimum_number_of_correspondences() const override
  {
    return 5;
  }

protected: // -- internal utilities --
  //: the main routine for carrying out the optimization. (used by the others)
  bool
  optimize_h(const std::vector<vgl_homg_point_2d<double>> & points1,
             const std::vector<vgl_homg_point_2d<double>> & points2,
             const vgl_h_matrix_2d<double> & h_initial,
             vgl_h_matrix_2d<double> & h_optimized);

  //: compute from matched points

  bool
  optimize_p(const std::vector<vgl_homg_point_2d<double>> & points1,
             const std::vector<vgl_homg_point_2d<double>> & points2,
             vgl_h_matrix_2d<double> & H) override;

  //: compute from matched lines

  bool
  optimize_l(const std::vector<vgl_homg_line_2d<double>> & lines1,
             const std::vector<vgl_homg_line_2d<double>> & lines2,
             vgl_h_matrix_2d<double> & H) override;

  //: compute from matched points and lines

  bool
  optimize_pl(const std::vector<vgl_homg_point_2d<double>> & points1,
              const std::vector<vgl_homg_point_2d<double>> & points2,
              const std::vector<vgl_homg_line_2d<double>> & lines1,
              const std::vector<vgl_homg_line_2d<double>> & lines2,
              vgl_h_matrix_2d<double> & H) override;
};

#endif // vgl_h_matrix_2d_optimize_lmq_h_
