#ifndef HMatrix1DCompute3Point_h_
#define HMatrix1DCompute3Point_h_
//:
// \file
//
// HMatrix1DComputeLinear contains a method to calculate
// the line projectivity which relates three 1D point correspondences.
//
// \author fsm
//

#include "HMatrix1DCompute.h"

class HMatrix1DCompute3Point : public HMatrix1DCompute
{
 public:
  HMatrix1DCompute3Point(void);
  ~HMatrix1DCompute3Point();
 protected:
  bool compute_cool_homg(const vcl_vector<HomgPoint1D> &,
                         const vcl_vector<HomgPoint1D> &,
                         HMatrix1D *);
};

#endif // HMatrix1DCompute3Point_h_
