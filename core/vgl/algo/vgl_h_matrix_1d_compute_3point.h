// This is core/vgl/algo/vgl_h_matrix_1d_compute_3point.h
#ifndef vgl_h_matrix_1d_compute_3point_h_
#define vgl_h_matrix_1d_compute_3point_h_
//:
// \file
// \brief Calculate the line projectivity which matches three 1D point correspondences
//
// \author
// F. Schaffalitzky, RRG
//
// \verbatim
//  Modifications
//   23 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//                 computations restricted to vgl_homg_point_1d<double>
//                 Seems somewhat overdoing it to template the transform
//                 solvers since double is needed for robust computation
//   23 Jun 2003 - Peter Vanroose - made compute_cool_homg pure virtual
// \endverbatim

#include <vgl/algo/vgl_h_matrix_1d_compute.h>

//: Calculate the line projectivity which matches three 1D point correspondences
//  To obtain the H-matrix from 3 point pairs, call the compute() method from
//  the parent class vgl_h_matrix_1d_compute.
class vgl_h_matrix_1d_compute_3point : public vgl_h_matrix_1d_compute
{
 public:
  vgl_h_matrix_1d_compute_3point(void) = default;
  ~vgl_h_matrix_1d_compute_3point() override = default;
 protected:
  bool
    compute_cool_homg(const std::vector<vgl_homg_point_1d<double> > & points1,
                      const std::vector<vgl_homg_point_1d<double> > & points2,
                      vgl_h_matrix_1d<double>& H) override;
};

#endif // vgl_h_matrix_1d_compute_3point_h_
