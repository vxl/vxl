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
  vnl_double_2 p,q;
  for (unsigned i=0;i<N;i++) {
    p=P[i].get_vector();
    q=Q[i].get_vector();
    D(i,0)=p(0)*q(1);
    D(i,1)=p(1)*q(1);
    D(i,2)=-p(0)*q(0);
    D(i,3)=-p(1)*q(0);
  }
  vnl_svd<double> svd(D);
  M->set(svd.nullvector().data_block());
  return true;
}
