// This is oxl/mvl/PMatrixDec.h
#ifndef PMatrixDec_h_
#define PMatrixDec_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
//
//   The PMatrixDec class is a subclass of PMatrix.
//   It justs adds decomposition of the projection matrix, P, into
//   2 matrices: J (3x3) and D (4x4), with intrinsic and extrinsic
//   parameters, respectively, where P=[J O_3]D.
//
//   References:
// \verbatim
//   pp 50 and 52-54, or more widely, CHAPTER 3 in (Faugeras, 1993):
//   @Book{        faugeras:93,
//     author    = {Faugeras, Olivier},
//     title     = {Three-Dimensional Computer Vision: a Geometric Viewpoint},
//     year      = {1993},
//     publisher = mit-press
//   }
// \endverbatim
//
// \author
//   Angeles Lopez (28-Apr-97)
//
// \verbatim
// Modifications:
//   15-May-97, A.Lopez -> Provide access methods for intrinsic
//                         parameters, denoted by AlphaU, AlphaV, U0 & V0.
// \endverbatim
//
//----------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <mvl/PMatrix.h>
#include <vcl_iosfwd.h>

class PMatrixDec : public PMatrix
{
  // Data Members------------------------------------------------------------

  // J and D matrices
  vnl_matrix<double> j_matrix_;  // 3x3
  vnl_matrix<double> d_matrix_;  // 4x4

 public:
  // Constructors/Initializers/Destructors----------------------------------
  PMatrixDec(const vnl_matrix<double>& p_matrix);
  ~PMatrixDec();

  // Data Access------------------------------------------------------------
  const vnl_matrix<double>& IntrinsicParameters () { return j_matrix_; }
  const vnl_matrix<double>& ExtrinsicParameters () { return d_matrix_; }

  double GetAlphaU() const { return j_matrix_(0,0); }
  double GetAlphaV() const { return j_matrix_(1,1); }
  double GetU0() const { return j_matrix_(0,2); }
  double GetV0() const { return j_matrix_(1,2); }

  // make tests for this class
  void Test();

  friend vcl_ostream& operator<<(vcl_ostream& s, const PMatrixDec& P);

  // INTERNALS---------------------------------------------------------------
 private:
  void Init();
};

#endif // PMatrixDec_h_
