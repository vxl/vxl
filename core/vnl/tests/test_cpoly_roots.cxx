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

#include <vnl/vnl_test.h>
#include <vnl/vnl_complex.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_real_polynomial.h>
#include <vnl/algo/vnl_cpoly_roots.h>

void testvnl_cpoly_roots()
{
  const double coeffs[] = {6, 5, 4, 3, 2, 1};
  vnl_vector<double> a(coeffs, 6);
  
  vnl_vector<double> monic( (a/a[0]).extract(a.size()-1,1) );
  vnl_cpoly_roots roots( monic, 0.0*monic );
  
  Assert( "Number of solutions", roots.solns.size() == monic.size() );
  
  // Evaluate results
  vnl_real_polynomial f(a);
  for(int i = 0; i < f.degree(); ++i)
    Assert("Root residual", vnl_math::abs(f.evaluate(roots.solns[i])) < 1e-12);
}

TESTMAIN(testvnl_cpoly_roots);
