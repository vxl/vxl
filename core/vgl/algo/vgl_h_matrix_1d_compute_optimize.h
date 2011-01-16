#ifndef vgl_h_matrix_1d_compute_optimize_h_
#define vgl_h_matrix_1d_compute_optimize_h_
//:
// \file
// \brief compute the h_matrix using Levenberg-Marquardt.
// \author F. Schaffalitzky, RRG
//
// \verbatim
// Modifications
//   23 Mar 2003 - J.L. Mundy - preparing for upgrade to vgl
//                 computations restricted to vgl_homg_point_1d<double>
//                 Seems somewhat overdoing it to template the transform
//                 solvers since double is needed for robust computation
//   23 Jun 2003 - Peter Vanroose - made compute_cool_homg pure virtual
// \endverbatim

#include <vgl/algo/vgl_h_matrix_1d_compute.h>

class vgl_h_matrix_1d_compute_optimize : public vgl_h_matrix_1d_compute
{
 public:
  vgl_h_matrix_1d_compute_optimize(void) {}
  ~vgl_h_matrix_1d_compute_optimize() {}
 protected:
  bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >& points1,
                         const vcl_vector<vgl_homg_point_1d<double> >& points2,
                         vgl_h_matrix_1d<double>& H);
};

typedef vgl_h_matrix_1d_compute_optimize vgl_h_matrix_1d_computeOptimise1;

#endif // vgl_h_matrix_1d_compute_optimize_h_
