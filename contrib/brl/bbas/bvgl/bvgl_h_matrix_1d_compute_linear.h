//---*-c++-*---
#ifndef _bvgl_h_matrix_1d_compute_linear_h_
#define _bvgl_h_matrix_1d_compute_linear_h_

// .NAME bvgl_h_matrix_1d_compute_linear
// .INCLUDE mvl/bvgl_h_matrix_1d_compute_linear.h
// .FILE bvgl_h_matrix_1d_compute_linear.cxx

#include "bvgl_h_matrix_1d_compute.h"

class bvgl_h_matrix_1d_compute_linear : public bvgl_h_matrix_1d_compute
{
 protected:
  bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> > &,
                         const vcl_vector<vgl_homg_point_1d<double> > &,
                         bvgl_h_matrix_1d<double>& H);
 public:
  bvgl_h_matrix_1d_compute_linear(void);
  ~bvgl_h_matrix_1d_compute_linear();
};

#endif // _bvgl_h_matrix_1d_compute_linear_h_
