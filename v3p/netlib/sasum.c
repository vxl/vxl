/* sasum.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

doublereal sasum_(n, sx, incx)
integer *n;
real *sx;
integer *incx;
{
    /* System generated locals */
    integer i__1, i__2;
    real ret_val;

    /* Local variables */
    static integer i, m, nincx;
    static real stemp;
    static integer mp1;


/*     takes the sum of the absolute values. */
/*     uses unrolled loops for increment equal to one. */
/*     jack dongarra, linpack, 3/11/78. */
/*     modified 3/93 to return if incx .le. 0. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */


    /* Parameter adjustments */
    --sx;

    /* Function Body */
    ret_val = (float)0.;
    stemp = (float)0.;
    if (*n <= 0 || *incx <= 0) {
        return ret_val;
    }
    if (*incx == 1) {
        goto L20;
    }

/*        code for increment not equal to 1 */

    nincx = *n * *incx;
    i__1 = nincx;
    i__2 = *incx;
    for (i = 1; i__2 < 0 ? i >= i__1 : i <= i__1; i += i__2) {
        stemp += dabs(sx[i]);
/* L10: */
    }
    ret_val = stemp;
    return ret_val;

/*        code for increment equal to 1 */


/*        clean-up loop */

L20:
    m = *n % 6;
    if (m == 0) {
        goto L40;
    }
    i__2 = m;
    for (i = 1; i <= i__2; ++i) {
        stemp += dabs(sx[i]);
/* L30: */
    }
    if (*n < 6) {
        goto L60;
    }
L40:
    mp1 = m + 1;
    i__2 = *n;
    for (i = mp1; i <= i__2; i += 6) {
        stemp = stemp + dabs(sx[i]) + dabs(sx[i + 1]) + dabs(sx[i + 2]) + dabs(sx[i + 3]) + dabs(sx[i + 4]) + dabs(sx[i + 5]);
/* L50: */
    }
L60:
    ret_val = stemp;
    return ret_val;
} /* sasum_ */

