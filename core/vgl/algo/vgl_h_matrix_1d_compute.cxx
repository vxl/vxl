// This is core/vgl/algo/vgl_h_matrix_1d_compute.cxx
#include "vgl_h_matrix_1d_compute.h"

bool
vgl_h_matrix_1d_compute::compute_array_dbl(const double p1[],
                                           const double p2[],
                                           unsigned int N,
                                           vgl_h_matrix_1d<double>& H)
{
  vcl_vector<vgl_homg_point_1d<double> > pt1; pt1.reserve(N);
  vcl_vector<vgl_homg_point_1d<double> > pt2; pt2.reserve(N);
  for (unsigned int i=0;i<N;i++) {
    pt1.push_back(vgl_homg_point_1d<double>(p1[i],1.0));
    pt2.push_back(vgl_homg_point_1d<double>(p2[i],1.0));
  }
  return compute_cool_homg(pt1,pt2,H); // pure virtual function
}
