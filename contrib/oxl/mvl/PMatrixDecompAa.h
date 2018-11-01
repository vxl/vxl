// This is oxl/mvl/PMatrixDecompAa.h
#ifndef PMatrixDecompAa_h_
#define PMatrixDecompAa_h_
//:
//  \file
// \brief Decompose PMatrix into [A a]
//
//    Decompose PMatrix into [A a] where A is 3x3 and a is 3x1.
//
// \author
//     Andrew W. Fitzgibbon, Oxford RRG, 14 Feb 97

#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_double_3.h>
#include <mvl/PMatrix.h>

class PMatrixDecompAa
{
 public:

//: Public data members for A and a.
  vnl_double_3x3 A;
  vnl_double_3   a;
  //{genman, do not do anything to this line -- awf}

//: Default constructor
  PMatrixDecompAa() = default;

//: Construct from PMatrix.
  PMatrixDecompAa(const PMatrix& P) { set(P); }

//: Construct from PMatrix.
  PMatrixDecompAa(const vnl_matrix<double>& P) { set(P); }

//:  Set [A a] from PMatrix.
  void set(const PMatrix& P) { P.get(&A.as_ref().non_const(), &a.as_ref().non_const()); }

//:  Set [A a] from PMatrix.
  void set(const vnl_matrix<double>& P);

//:  Set PMatrix from [A a].
  void get(PMatrix* P) const { P->set(A, a); }
};

#endif // PMatrixDecompAa_h_
