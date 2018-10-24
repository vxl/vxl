#ifndef _HMatrix2DCompute4Line_
#define _HMatrix2DCompute4Line_
//--------------------------------------------------------------
//:
// \file
//
// HMatrix2DCompute4Line computes the homography relating
// two sets of four homogeneous lines in general position.
//
// \author fsm, Oxford RRG, 8 February 1998

#include "HMatrix2DCompute.h"

class HMatrix2DCompute4Line : public HMatrix2DCompute {
//--------------------------------------------------------------------------------
protected:
  bool compute_l(const LineArray&,
                 const LineArray&,
                 HMatrix2D *) override;
//--------------------------------------------------------------------------------
public:
  int minimum_number_of_correspondences() const override { return 4; }
};

#endif // _HMatrix2DCompute4Line_
