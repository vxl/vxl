#ifndef _HMatrix2DCompute4Line_
#define _HMatrix2DCompute4Line_
//--------------------------------------------------------------
//
// .NAME HMatrix2DCompute4Line
// .LIBRARY MViewCompute
// .HEADER MultiView package
// .INCLUDE mvl/HMatrix2DCompute4Line.h
// .FILE HMatrix2DCompute4Line.cxx
//
// .SECTION Description:
// HMatrix2DCompute4Line computes the homography relating
// two sets of four homogeneous lines in general position.
//
// .SECTION Author
//     Frederik Schaffalitzky, Oxford RRG, 8 February 1998
//

#include "HMatrix2DCompute.h"

class HMatrix2DCompute4Line : public HMatrix2DCompute {
//--------------------------------------------------------------------------------
protected:
  bool compute_l(const LineArray&,
                 const LineArray&,
                 HMatrix2D *);
//--------------------------------------------------------------------------------
public:

};

#endif // _HMatrix2DCompute4Line_
