#ifndef _HMatrix1DComputeDesign_h_
#define _HMatrix1DComputeDesign_h_

#include <vgl/vgl_fwd.h>
#include "HMatrix1DCompute.h"

class HMatrix1DComputeDesign : public HMatrix1DCompute
{
 protected:
  bool compute_cool_homg(const std::vector<vgl_homg_point_1d<double> >&,
                         const std::vector<vgl_homg_point_1d<double> >&,
                         HMatrix1D *) override;
 public:
  HMatrix1DComputeDesign(void);
  ~HMatrix1DComputeDesign() override;
};

#endif // _HMatrix1DComputeDesign_h_
