#ifndef _PMatrixDec_h
#define _PMatrixDec_h
#ifdef __GNUC__
#pragma interface
#endif

//-*- c++ -*----------------------------------------------------------------
//
// .NAME PMatrixDec
//
// .SECTION Description
//   The PMatrixDec class is a subclass of PMatrix.
//   It justs adds decomposition of the projection matrix, P, into
//   2 matrices: J (3x3) and D (4x4), with intrinsic and extrinsic
//   parameters, respectively, where P=[J O_3]D.
//
//   References:
//
//   pp 50 and 52-54, or more widely, CHAPTER 3 in (Faugeras, 1993):
//   @Book{        faugeras:93,
//     author    = {Faugeras, Olivier},
//     title     = {Three-Dimensional Computer Vision: a Geometric Viewpoint},
//     year      = {1993},
//     publisher = mit-press
//   }
//
// .SECTION Author
//   Angeles Lopez (28-Apr-97)
//
// .SECTION Modifications:
//   15-May-97, A.Lopez -> Provide access methods for intrinsic
//                         parameters, denoted by AlphaU, AlphaV, U0 & V0.
//
//----------------------------------------------------------------------------

#include <vnl/vnl_matrix.h>
#include <mvl/PMatrix.h>
#include <vcl/vcl_iosfwd.h>


class PMatrixDec : public PMatrix {

  // PUBLIC INTERFACE-------------------------------------------------------
public:

  // Constructors/Initializers/Destructors----------------------------------
  PMatrixDec(const vnl_matrix<double>& p_matrix);
  ~PMatrixDec();

  // Data Access------------------------------------------------------------
  const vnl_matrix<double>& IntrinsicParameters () { return _j_matrix; }
  const vnl_matrix<double>& ExtrinsicParameters () { return _d_matrix; }

  double GetAlphaU() const { return _j_matrix(0,0); }
  double GetAlphaV() const { return _j_matrix(1,1); }
  double GetU0() const { return _j_matrix(0,2); }
  double GetV0() const { return _j_matrix(1,2); }

  // make tests for this class
  void Test();

  friend ostream& operator<<(ostream& s, const PMatrixDec& P);

  // INTERNALS---------------------------------------------------------------
protected:

  // Data Members------------------------------------------------------------

  // J and D matrices
  vnl_matrix<double> _j_matrix;  // 3x3
  vnl_matrix<double> _d_matrix;  // 4x4

private:
  void Init();
};

#endif // _PMatrixDec_h
