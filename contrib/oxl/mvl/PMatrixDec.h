// <begin copyright notice>
// ---------------------------------------------------------------------------
//
//                   Copyright (c) 1997 TargetJr Consortium
//               GE Corporate Research and Development (GE CRD)
//                             1 Research Circle
//                            Niskayuna, NY 12309
//                            All Rights Reserved
//              Reproduction rights limited as described below.
//                               
//      Permission to use, copy, modify, distribute, and sell this software
//      and its documentation for any purpose is hereby granted without fee,
//      provided that (i) the above copyright notice and this permission
//      notice appear in all copies of the software and related documentation,
//      (ii) the name TargetJr Consortium (represented by GE CRD), may not be
//      used in any advertising or publicity relating to the software without
//      the specific, prior written permission of GE CRD, and (iii) any
//      modifications are clearly marked and summarized in a change history
//      log.
//       
//      THE SOFTWARE IS PROVIDED "AS IS" AND WITHOUT WARRANTY OF ANY KIND,
//      EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
//      WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
//      IN NO EVENT SHALL THE TARGETJR CONSORTIUM BE LIABLE FOR ANY SPECIAL,
//      INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND OR ANY
//      DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
//      WHETHER OR NOT ADVISED OF THE POSSIBILITY OF SUCH DAMAGES, OR ON
//      ANY THEORY OF LIABILITY ARISING OUT OF OR IN CONNECTION WITH THE
//      USE OR PERFORMANCE OF THIS SOFTWARE.
//
// ---------------------------------------------------------------------------
// <end copyright notice>
//-*- c++ -*----------------------------------------------------------------
//
// Class : PMatrixDec
//
// Description
//   The PMatrixDec class is a subclass of PMatrix.
//   It justs adds decomposition of the projection matrix, P, into
//   2 matrices: J (3x3) and D (4x4), with intrinsic and extrinsic 
//   parameters, respectively, where P=[J O_3]D.
//
//   References:
//
//   pp 50 and 52-54, or more widely, CHAPTER 3 in (Faugeras, 1993):
//   @Book{            faugeras:93,
//     author        = {Faugeras, Olivier},
//     title         = {Three-Dimensional Computer Vision: a Geometric
//                     Viewpoint},
//     year          = {1993},
//     publisher     = mit-press
//   }
//
// Author: Angeles Lopez (28-Apr-97)
//
// Modifications:
//   15-May-97, A.Lopez -> Provide access methods for intrinsic
//                         parameters, denoted by AlphaU, AlphaV, U0 & V0.
//
// Example:
//
//----------------------------------------------------------------------------

#ifndef _PMatrixDec_h
#define _PMatrixDec_h

#ifdef __GNUC__
#pragma interface
#endif
  
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

#endif
