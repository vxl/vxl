//---*-c++-*---
#ifndef _vgl_h_matrix_1d_compute_linear_h_
#define _vgl_h_matrix_1d_compute_linear_h_

// .NAME vgl_h_matrix_1d_compute_linear
// .INCLUDE mvl/vgl_h_matrix_1d_compute_linear.h
// .FILE vgl_h_matrix_1d_compute_linear.cxx

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

#endif // _vgl_h_matrix_1d_compute_linear_h_
