// This is oxl/mvl/PMatrixDecompCR.h
#ifndef PMatrixDecompCR_h_
#define PMatrixDecompCR_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Decompose PMatrix into calibration + rotation
//
//    PMatrixDecompCR decomposes a projection matrix into the form
//    K [R t] where K is upper triangular and R is a rotation.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 08 Jun 98
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>

class PMatrix;

struct PMatrixDecompCR
{
  vnl_double_3x3 C;
  vnl_double_3x4 Po;

  PMatrixDecompCR(const vnl_matrix<double>& P_3x4, bool scale_C = true);
  PMatrixDecompCR(const PMatrix& P, bool scale_C = true);

  void compute(const vnl_matrix<double>& P_3x4, bool scale_C = true);
};

#endif // PMatrixDecompCR_h_
