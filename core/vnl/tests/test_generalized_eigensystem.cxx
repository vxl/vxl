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
#ifdef __GNUC__
#pragma implementation "test_generalized_eigensystem.h"
#endif
//
// Class: test_generalized_eigensystem
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_matops.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>

extern "C"
void test_generalized_eigensystem()
{
  double Sdata[36] = {
   30.0000,   -3.4273,   13.9254,   13.7049,   -2.4446,   20.2380,
   -3.4273,   13.7049,   -2.4446,    1.3659,    3.6702,   -0.2282,
   13.9254,   -2.4446,   20.2380,    3.6702,   -0.2282,   28.6779,
   13.7049,    1.3659,    3.6702,   12.5273,   -1.6045,    3.9419,
   -2.4446,    3.6702,   -0.2282,   -1.6045,    3.9419,    2.5821,
   20.2380,   -0.2282,   28.6779,    3.9419,    2.5821,   44.0636,
  };
  double Cdata[36] = {
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  0,  
    0,  0,  0,  0,  0,  2,  
    0,  0,  0,  0, -1,  0,  
    0,  0,  0,  2,  0,  0,  
  };

  vnl_matrix<double> S(Sdata, 6,6);
  vnl_matrix<double> C(Cdata, 6,6);
  
  vnl_generalized_eigensystem gev(C, S);

  cout << "V = " << gev.V << endl;
  cout << "D = " << gev.D << endl;
  cout << "residual = " << C * gev.V - S * gev.V * gev.D << endl;
  double err = (C * gev.V - S * gev.V * gev.D).inf_norm();
  cout << "Recomposition residual = " << err << endl;

  Assert("Recomposition residual < 1e-12", err < 1e-12);
}

TESTMAIN(test_generalized_eigensystem);
