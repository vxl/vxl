// This is oxl/mvl/PMatrixDecompCR.h
#ifndef PMatrixDecompCR_h_
#define PMatrixDecompCR_h_
//:
// \file
// \brief Decompose PMatrix into calibration + rotation
//
//    PMatrixDecompCR decomposes a projection matrix into the form
//    C . Po = C . [R t] , where C is upper triangular and R is a rotation.
//    The diagonal elements of C are guaranteed to be nonnegative.
//    Moreover, if the optional second parameter of the constructor
//    is not set to "false", the matrix C is scaled so that C(2,2) = 1.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 08 Jun 98
//
// \verbatim
// Modifications:
//   14-Nov-02, Peter Vanroose - make sure C(1,1) is nonnegative
// \endverbatim
//-----------------------------------------------------------------------------

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3x4.h>
class PMatrix;

struct PMatrixDecompCR
{
  vnl_double_3x3 C;
  vnl_double_3x4 Po;

  PMatrixDecompCR(PMatrix const& P, bool scale_C = true);
 private:
  PMatrixDecompCR(vnl_double_3x4 const& P_3x4, bool scale_C);
  void compute(vnl_double_3x4 const& P_3x4, bool scale_C);
};

#endif // PMatrixDecompCR_h_
