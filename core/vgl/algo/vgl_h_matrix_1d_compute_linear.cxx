#include "vgl_h_matrix_1d_compute_linear.h"

#include <vcl_cassert.h>

#include <vnl/algo/vnl_svd.h>
#include <vgl/algo/vgl_h_matrix_1d.h>

//********************************************************************************
//
//
//
//********************************************************************************


vgl_h_matrix_1d_compute_linear::
vgl_h_matrix_1d_compute_linear(void) : vgl_h_matrix_1d_compute() { }

vgl_h_matrix_1d_compute_linear::~vgl_h_matrix_1d_compute_linear() { }

bool vgl_h_matrix_1d_compute_linear::
compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >&P,
                  const vcl_vector<vgl_homg_point_1d<double> >&Q,
                  vgl_h_matrix_1d<double>& M)
{
  unsigned N=P.size(); assert(N==Q.size());
  vnl_matrix<double> D(N,4);
  for (unsigned i=0;i<N;i++) {
    D(i,0)=P[i].x()*Q[i].w();
    D(i,1)=P[i].w()*Q[i].w();
    D(i,2)=-P[i].x()*Q[i].x();
    D(i,3)=-P[i].w()*Q[i].x();
  }
  vnl_svd<double> svd(D);
  M.set(svd.nullvector().data_block());
  return true;
}
