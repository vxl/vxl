// This is core/vgl/algo/vgl_h_matrix_1d_compute_linear.h
#ifndef vgl_h_matrix_1d_compute_linear_h_
#define vgl_h_matrix_1d_compute_linear_h_
//:
// \file
// \brief find line-to-line projectivity from a set of matched points using SVD
//
// \verbatim
//  Modifications
//   23 Jun 2003 - Peter Vanroose - made compute_cool_homg pure virtual
// \endverbatim

#include "vgl_h_matrix_1d_compute.h"

class vgl_h_matrix_1d_compute_linear : public vgl_h_matrix_1d_compute
{
 public:
  vgl_h_matrix_1d_compute_linear(void) {}
  ~vgl_h_matrix_1d_compute_linear() {}
 protected:
  virtual bool
    compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > & points1,
                      const vcl_vector<vgl_homg_point_1d<double> > & points2,
                      vgl_h_matrix_1d<double>& H);
};

#endif // vgl_h_matrix_1d_compute_linear_h_
