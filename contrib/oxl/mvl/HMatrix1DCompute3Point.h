#ifndef _HMatrix1DCompute3Point_h_
#define _HMatrix1DCompute3Point_h_

//--------------------------------------------------------------
//
// .NAME HMatrix1DCompute3Point
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix1DCompute3Point.h
// .FILE HMatrix1DCompute3Point.cxx
//
// .SECTION Description:
//
// HMatrix1DComputeLinear contains a method to calculate
// the line projectivity which relates three 1D point correspondences.
//
// .SECTION Author
// F. Schaffalitzky, RRG
// .SECTION Modifications:
//

#include "HMatrix1DCompute.h"

class HMatrix1DCompute3Point : public HMatrix1DCompute {
//--------------------------------------------------------------------------------
 protected:
  bool compute_cool_homg(const vcl_vector<HomgPoint1D> &,
			 const vcl_vector<HomgPoint1D> &,
			 HMatrix1D *);
  //--------------------------------------------------------------------------------
 public:
  HMatrix1DCompute3Point(void);
  ~HMatrix1DCompute3Point();
};

#endif // _HMatrix1DCompute3Point_h_
