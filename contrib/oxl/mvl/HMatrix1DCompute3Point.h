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

#include <vgl/vgl_fwd.h>
#include "HMatrix1DCompute.h"

class HMatrix1DCompute3Point : public HMatrix1DCompute
{
 public:
  HMatrix1DCompute3Point(void);
  ~HMatrix1DCompute3Point() override;
 protected:
  bool compute_cool_homg(const std::vector<vgl_homg_point_1d<double> >&,
                         const std::vector<vgl_homg_point_1d<double> >&,
                         HMatrix1D *) override;
};

#endif // HMatrix1DCompute3Point_h_
