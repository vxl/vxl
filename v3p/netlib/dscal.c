/* dscal.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Subroutine */ void dscal_(n, da, dx, incx)
integer *n;
doublereal *da, *dx;
integer *incx;
{
    /* Local variables */
    static integer i, m, nincx;

/*     scales a vector by a constant.					*/
/*     uses unrolled loops for increment equal to one.			*/
/*     jack dongarra, linpack, 3/11/78.					*/
/*     modified 3/93 to return if incx .le. 0.				*/
/*     modified 12/3/93, array(1) declarations changed to array(*)	*/

    if (*n <= 0 || *incx <= 0) {
        return;
    }
    if (*incx == 1) {
        goto L20;
    }

/*        code for increment not equal to 1 */

    nincx = *n * *incx;
    for (i = 0; i < nincx; i += *incx) {
        dx[i] = *da * dx[i];
    }
    return;

/*        code for increment equal to 1 */

/*        clean-up loop */

L20:
    m = *n % 5;
    if (m == 0) {
        goto L40;
    }
    for (i = 0; i < m; ++i) {
        dx[i] = *da * dx[i];
    }
    if (*n < 5) {
        return;
    }
L40:
    for (i = m; i < *n; i += 5) {
        dx[i] = *da * dx[i];
        dx[i+1] = *da * dx[i+1];
        dx[i+2] = *da * dx[i+2];
        dx[i+3] = *da * dx[i+3];
        dx[i+4] = *da * dx[i+4];
    }
} /* dscal_ */

