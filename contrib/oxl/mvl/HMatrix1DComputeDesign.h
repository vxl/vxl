#ifndef _HMatrix1DComputeDesign_h_
#define _HMatrix1DComputeDesign_h_


#include "HMatrix1DCompute.h"

class HMatrix1DComputeDesign : public HMatrix1DCompute
{
 protected:
  bool compute_cool_homg(const vcl_vector<HomgPoint1D> &,
			 const vcl_vector<HomgPoint1D> &,
			 HMatrix1D *);
 public:
  HMatrix1DComputeDesign(void);
  ~HMatrix1DComputeDesign();
};

#endif
