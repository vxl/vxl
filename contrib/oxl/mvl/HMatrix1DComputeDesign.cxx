#include "HMatrix1DComputeDesign.h"

#include <vcl_cassert.h>

#include <vnl/algo/vnl_svd.h>
#include <mvl/HMatrix1D.h>

//********************************************************************************
//
//
//
//********************************************************************************


HMatrix1DComputeDesign::HMatrix1DComputeDesign(void) : HMatrix1DCompute() { }
HMatrix1DComputeDesign::~HMatrix1DComputeDesign() { }

bool
HMatrix1DComputeDesign::compute_cool_homg(const vcl_vector<HomgPoint1D>&P,
                                          const vcl_vector<HomgPoint1D>&Q,
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
