/* <begin copyright notice>
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
*/
#include <stdio.h>
#include <assert.h>
#include <vnl/vnl_math.h> /* for fabs() */

extern "C"
{
int sggsvd_(char *jobu, char *jobv, char *jobq, int *m, int *n, int *p,
            int *k, int *l, float *a, int *lda, float *b, int *ldb,
            float *alpha, float *beta, float *u, int *ldu, float *v,
            int *ldv, float *q, int *ldq, float *work, int *iwork,
            int *info);

}
int main() {
  float A[9]={2./3, -1.36/3, .2/3,   2.8/3, .4/3, 1./3,   1, .16, -.2};
  float AA[9]={2./3, -1.36/3, .2/3,   2.8/3, .4/3, 1./3,   1, .16, -.2};
  float B[9]={.16, -.224, -.768,   .8, .36, -.48,  1.12, -.168, -.576};
  float BB[9]={.16, -.224, -.768,   .8, .36, -.48,  1.12, -.168, -.576};
  float U[9], V[9], Q[9], Alpha[3], Beta[3], Work[12];
  int m=3, n=3, p=3, k, l, Iwork[3], info, tests_failed=0, tests_succeeded=0;

  sggsvd_("U", "V", "Q", &m, &n, &p, &k, &l, AA, &n, BB, &n, Alpha, Beta,
          U, &n, V, &n, Q, &n, Work, Iwork, &info);

  printf("k = %d, l = %d, info = %d\n", k, l, info);
  if (k!=0 || l!=3) {
    printf("*** Failed: (k,l) must be (0,3), not (%1d,%1d)\n", k, l);
    ++tests_failed;
  } else ++tests_succeeded;
  if (info!=0) {
    printf("*** Failed: sggsvd returned %1d instead of 0\n", info);
    ++tests_failed;
  } else ++tests_succeeded;

  printf("U = %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n",
         U[0], U[3], U[6], U[1], U[4], U[7], U[2], U[5], U[8]);
  printf("V = %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n",
         V[0], V[3], V[6], V[1], V[4], V[7], V[2], V[5], V[8]);
  printf("Q = %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n",
         Q[0], Q[3], Q[6], Q[1], Q[4], Q[7], Q[2], Q[5], Q[8]);
  printf("D1 = diag(%12g %12g %12g)\n", Alpha[0], Alpha[1], Alpha[2]);
  printf("D2 = diag(%12g %12g %12g)\n", Beta[0], Beta[1], Beta[2]);
  printf("R = %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n    %12.7f %12.7f %12.7f\n",
         AA[0], AA[3], AA[6], AA[1], AA[4], AA[7], AA[2], AA[5], AA[8]);

  if (fabs(Alpha[0]-0.6)>1e-6 || fabs(Alpha[1]-0.8)>1e-6 || fabs(Alpha[2]-0.6)>1e-6) {
    printf("*** Failed: D1 must be (0.6,0.8,0.6), not (%g,%g,%g)\n",
           Alpha[0], Alpha[1], Alpha[2]);
    ++tests_failed;
  } else ++tests_succeeded;

  if (fabs(Beta[0]-0.8)>1e-6 || fabs(Beta[1]-0.6)>1e-6 || fabs(Beta[2]-0.8)>1e-6) {
    printf("*** Failed: D2 must be (0.8,0.6,0.8), not (%g,%g,%g)\n",
           Beta[0], Beta[1], Beta[2]);
    ++tests_failed;
  } else ++tests_succeeded;

  if (tests_failed == 0)
    printf("testQvnl_svd<double> Test Summary: All %1d tests succeeded\n", tests_succeeded);
  else if (tests_failed == 1)
    printf("testQvnl_svd<double> Test Summary: %1d tests succeeded, 1 test failed ***\n", tests_succeeded);
  else if (tests_succeeded == 1)
    printf("testQvnl_svd<double> Test Summary: 1 test succeeded, %1d tests failed ***\n", tests_failed);
  else if (tests_succeeded == 0)
    printf("testQvnl_svd<double> Test Summary: all %1d tests failed ***\n", tests_failed);
  else
    printf("testQvnl_svd<double> Test Summary: %1d tests succeeded, %1d tests failed ***\n", tests_succeeded, tests_failed);

  return tests_failed;
}
