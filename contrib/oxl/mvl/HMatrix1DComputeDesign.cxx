#include "HMatrix1DComputeDesign.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_1d.h>
#include <mvl/HMatrix1D.h>

//********************************************************************************
//
//
//
//********************************************************************************


HMatrix1DComputeDesign::HMatrix1DComputeDesign(void) : HMatrix1DCompute() { }
HMatrix1DComputeDesign::~HMatrix1DComputeDesign() = default;

bool
HMatrix1DComputeDesign::compute_cool_homg(const std::vector<vgl_homg_point_1d<double> >&P,
                                          const std::vector<vgl_homg_point_1d<double> >&Q,
                                          HMatrix1D *M)
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
  M->set(svd.nullvector().data_block());
  return true;
}
