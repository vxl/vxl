#ifndef _HMatrix1DComputeOptimize1_h_
#define _HMatrix1DComputeOptimize1_h_

#include "HMatrix1DCompute.h"

class HMatrix1DComputeOptimize1 : public HMatrix1DCompute
{
 protected:
  bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >&,
                         const vcl_vector<vgl_homg_point_1d<double> >&,
                         HMatrix1D *);
 public:
  HMatrix1DComputeOptimize1(void);
  ~HMatrix1DComputeOptimize1();
};

typedef HMatrix1DComputeOptimize1 HMatrix1DComputeOptimise1;

#endif // _HMatrix1DComputeOptimize1_h_
