#ifndef _HMatrix1DComputeDesign_h_
#define _HMatrix1DComputeDesign_h_

#include "HMatrix1DCompute.h"

class HMatrix1DComputeDesign : public HMatrix1DCompute
{
 protected:
  bool compute_cool_homg(const vcl_vector<vgl_homg_point_1d<double> >&,
                         const vcl_vector<vgl_homg_point_1d<double> >&,
                         HMatrix1D *);
 public:
  HMatrix1DComputeDesign(void);
  ~HMatrix1DComputeDesign();
};

#endif // _HMatrix1DComputeDesign_h_
