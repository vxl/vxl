/*  -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

integer izmax1_(n, cx, incx)
integer *n;
doublecomplex *cx;
integer *incx;
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    static doublereal smax;
    static integer i, ix;


/*  -- LAPACK auxiliary routine (version 2.0) --			*/
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd.,	*/
/*     Courant Institute, Argonne National Lab, and Rice University	*/
/*     September 30, 1994						*/
/*									*/
/*     .. Scalar Arguments ..						*/
/*     ..								*/
/*     .. Array Arguments ..						*/
/*     ..								*/
/*									*/
/*  Purpose								*/
/*  =======								*/
/*									*/
/*  IZMAX1 finds the index of the element whose real part has maximum	*/
/*  absolute value.							*/
/*									*/
/*  Based on IZAMAX from Level 1 BLAS.					*/
/*  The change is to use the 'genuine' absolute value.			*/
/*									*/
/*  Contributed by Nick Higham for use with ZLACON.			*/
/*									*/
/*  Arguments								*/
/*  =========								*/
/*									*/
/*  N       (input) INTEGER						*/
/*          The number of elements in the vector CX.			*/
/*									*/
/*  CX      (input) COMPLEX*16 array, dimension (N)			*/
/*          The vector whose elements will be summed.			*/
/*									*/
/*  INCX    (input) INTEGER						*/
/*          The spacing between successive values of CX.  INCX >= 1.	*/
/*									*/
/* =====================================================================*/

    if (*n < 1) {
        return 0;
    }
    if (*n == 1) {
        return 1;
    }
    if (*incx == 1) {
        goto L30;
    }

/*     CODE FOR INCREMENT NOT EQUAL TO 1 */

    ix = 0;
    smax = abs(cx[0].r);
    ix += *incx;
    for (i = 1; i < *n; ++i, ix += *incx) {
        if (abs(cx[ix].r) > smax) {
            ret_val = i+1;
            smax = abs(cx[ix].r);
        }
    }
    return ret_val;

/*     CODE FOR INCREMENT EQUAL TO 1 */

L30:
    smax = abs(cx[0].r);
    for (i = 1; i < *n; ++i) {
        if (abs(cx[i].r) > smax) {
            ret_val = i+1;
            smax = abs(cx[i].r);
        }
    }
    return ret_val;

} /* izmax1_ */

