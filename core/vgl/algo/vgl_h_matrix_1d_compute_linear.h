// This is core/vgl/algo/vgl_h_matrix_1d_compute_linear.h
#ifndef vgl_h_matrix_1d_compute_linear_h_
#define vgl_h_matrix_1d_compute_linear_h_
//:
// \file
// \brief find line-to-line projectivity from a set of matched points using SVD

#include "vgl_h_matrix_1d_compute.h"

class vgl_h_matrix_1d_compute_linear : public vgl_h_matrix_1d_compute
{
 protected:
  bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > &,
                         const vcl_vector<vgl_homg_point_1d<double> > &,
                         vgl_h_matrix_1d<double>& H);
 public:
  vgl_h_matrix_1d_compute_linear(void);
  ~vgl_h_matrix_1d_compute_linear();
};

#endif // vgl_h_matrix_1d_compute_linear_h_
