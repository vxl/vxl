#ifndef FMatrixCompute8Point_h_
#define FMatrixCompute8Point_h_
#ifdef __GNUC__
#pragma interface
#endif
//-----------------------------------------------------------------------------
//:
// \file
// \brief FMatrix by linear least squares
//
//    FMatrixCompute8Point is a subclass of FMatrixCompute, and implements
//    the ``8-point'' or linear least squares algorithm for estimation of
//    the fundamental matrix.
//
// \example examples/exampleFMatrixCompute.cxx
//
// \author
//     Andrew W. Fitzgibbon, Oxford IERG, 23 Jul 96

#include <mvl/FMatrix.h>
#include <mvl/FMatrixCompute.h>

class FMatrixCompute8Point : public FMatrixCompute {
public:
  // Implement FMatrixCompute interface
  bool compute();
};

#endif // FMatrixCompute8Point_h_
