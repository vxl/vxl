#include <iostream>
#include <cmath>
#include "ComputeGRIC.h"
#include <mvl/FMatrixComputeMLESAC.h>
#include <mvl/HMatrix2DComputeMLESAC.h>
#include <mvl/HomgInterestPointSet.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

ComputeGRIC::ComputeGRIC(double std) : std_(std) {}

ComputeGRIC::~ComputeGRIC()= default;

bool ComputeGRIC::compute(PairMatchSetCorner* matches) {

  // Make some copies of the corner matches so they are
  // not violated
  PairMatchSetCorner matches_copy1(*matches);
  PairMatchSetCorner matches_copy2(*matches);

  // Compute the two differing relations using MLE
  F_.reset(new FMatrix);
  std::unique_ptr<FMatrixComputeRobust> computor1(new FMatrixComputeMLESAC(true, std_));
  computor1->compute(matches_copy1, F_.get());
  residualsF_ = computor1->get_residuals();
  inliersF_ = computor1->get_inliers();
  basisF_ = computor1->get_basis();
  computor1.reset();

  H_.reset(new HMatrix2D);
  std::unique_ptr<HMatrix2DComputeRobust> computor2(new HMatrix2DComputeMLESAC(std_));
  computor2->compute(matches_copy2, H_.get());
  residualsH_ = computor2->get_residuals();
  inliersH_ = computor2->get_inliers();
  basisH_ = computor2->get_basis();
  computor2.reset();

  std::cerr << "Finished calculations" << std::endl;

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
  std::cerr << "inf : " << inf << std::endl;
  std::cerr << "inh : " << inh << std::endl;
  std::cerr << "stdf : " << stdf << std::endl;
  std::cerr << "stdh : " << stdh << std::endl;
  int df = 3, dh = 2, r = 4, kf = 7, kh = 8;
  double l1 = std::log(4.0), l2 = std::log(4.0*n), l3 = 2.0;
  double GRICF = 0.0;
  double thresh = l3*(r - df);
  for (double i : residualsF_) {
    double t = i / std_;
    if (t < thresh)
      GRICF += t;
    else
      GRICF += thresh;
  }
  GRICF += l1*(df*n) + l2*kf;
  std::cerr << "GRICF : " << GRICF << std::endl;
  double GRICH = 0.0;
  thresh = l3*(r - dh);
  for (double i : residualsH_) {
    double t = i / std_;
    if (t < thresh)
      GRICH += t;
    else
      GRICH += thresh;
  }
  GRICH += l1*(dh*n) + l2*kh;
  std::cerr << "GRICH : " << GRICH << std::endl;

  // Determine the winner
  if (GRICH < GRICF) {
    degenerate_ = true;
    return false;
  } else {
    degenerate_ = false;
    return true;
  }
}
