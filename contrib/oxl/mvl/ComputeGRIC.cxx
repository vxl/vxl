#include "ComputeGRIC.h"
#include <mvl/FMatrixComputeMLESAC.h>
#include <mvl/HMatrix2DComputeMLESAC.h>
#include <mvl/HomgInterestPointSet.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>

ComputeGRIC::ComputeGRIC(double std) : std_(std) {}

ComputeGRIC::~ComputeGRIC(){}

bool ComputeGRIC::compute(PairMatchSetCorner* matches) {

  // Make some copies of the corner matches so they are
  // not violated
  PairMatchSetCorner matches_copy1(*matches);
  PairMatchSetCorner matches_copy2(*matches);
  FMatrix *F = new FMatrix();
  HMatrix2D* H = new HMatrix2D();

  // Compute the two differing relations using MLE
  FMatrixComputeRobust* computor1 = new FMatrixComputeMLESAC(true, std_);
  HMatrix2DComputeRobust* computor2 = new HMatrix2DComputeMLESAC(std_);
  computor2->compute(matches_copy2, H);
  residualsH_ = computor2->get_residuals();
  inliersH_ = computor2->get_inliers();
  basisH_ = computor2->get_basis();
  H_ = H;
  delete computor2;
  computor1->compute(matches_copy1, F);
  residualsF_ = computor1->get_residuals();
  inliersF_ = computor1->get_inliers();
  basisF_ = computor1->get_basis();
  F_ = F;
  delete computor1;
  vcl_cerr << "Finished calculations" << vcl_endl;

  // Compare the GRIC scores of the two different models
  int inf = 0, inh = 0;
  double stdf = 0.0, stdh = 0.0;
  int n = 0;
  for (unsigned int i = 0; i < inliersF_.size(); i++) {
    n++;
    if (inliersF_[i]) {
      inf++;
      stdf += residualsF_[i];
    }
  }
  for (unsigned int i = 0; i < inliersH_.size(); i++) {
    if (inliersH_[i]) {
      inh++;
      stdh += residualsH_[i];
    }
  }
  stdf /= inf;
  stdh /= inh;
  vcl_cerr << "inf : " << inf << vcl_endl;
  vcl_cerr << "inh : " << inh << vcl_endl;
  vcl_cerr << "stdf : " << stdf << vcl_endl;
  vcl_cerr << "stdh : " << stdh << vcl_endl;
  int df = 3, dh = 2, r = 4, kf = 7, kh = 8;
  double l1 = vcl_log(4.0), l2 = vcl_log(4.0*n), l3 = 2.0;
  double GRICF = 0.0;
  double thresh = l3*(r - df);
  for (unsigned int i = 0; i < residualsF_.size(); i++) {
    double t = residualsF_[i] / std_;
    if (t < thresh)
      GRICF += t;
    else
      GRICF += thresh;
  }
  GRICF += l1*(df*n) + l2*kf;
  vcl_cerr << "GRICF : " << GRICF << vcl_endl;
  double GRICH = 0.0;
  thresh = l3*(r - dh);
  for (unsigned int i = 0; i < residualsH_.size(); i++) {
    double t = residualsH_[i] / std_;
    if (t < thresh)
      GRICH += t;
    else
      GRICH += thresh;
  }
  GRICH += l1*(dh*n) + l2*kh;
  vcl_cerr << "GRICH : " << GRICH << vcl_endl;

  // Determine the winner
  if (GRICH < GRICF) {
    degenerate_ = true;
    return false;
  } else {
    degenerate_ = false;
    return true;
  }
}
