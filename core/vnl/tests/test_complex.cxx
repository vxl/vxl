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

#include <vnl/vnl_complex.h>
#include <vnl/vnl_test.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_matops.h>
#include <vcl/vcl_cstdlib.h> // for rand()

// make a vector with random, complex entries :
static void fill_rand(vnl_double_complex *b, vnl_double_complex *e) {
  for (vnl_double_complex *p=b; p<e; ++p)
    (*p) = vnl_double_complex( 2*(rand()/double(RAND_MAX))-1, 2*(rand()/double(RAND_MAX))-1 );
}

// Driver
void test_complex() {
  vnl_vector<vnl_double_complex> a(5); fill_rand(a.begin(), a.end());
  vnl_vector<vnl_double_complex> b(5); fill_rand(b.begin(), b.end());

  cerr << "a=" << a << endl;
  cerr << "b=" << b << endl;

  vnl_double_complex i(0,1);
  
  Assert("inner_product() conjugates correctly", 
	 vnl_math_abs( inner_product(i*a,b)-i*inner_product(a,b) ) < 1e-12 &&
	 vnl_math_abs( inner_product(a,i*b)+i*inner_product(a,b) ) < 1e-12 );
  
  Assert("dot_product() does not conjugate", 
	 vnl_math_abs( dot_product(i*a,b)-i*dot_product(a,b) ) < 1e-12 &&
	 vnl_math_abs( dot_product(a,i*b)-i*dot_product(a,b) ) < 1e-12 );

  double norma=0;
  for (unsigned n=0; n<a.size(); ++n)
    norma += a[n].real()*a[n].real() + a[n].imag()*a[n].imag();
  norma = sqrt(norma);
  Assert("correct magnitude", vnl_math_abs( norma-a.magnitude() ) < 1e-12 );
}

TESTMAIN(test_complex);
