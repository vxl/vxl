/* sscal.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Subroutine */ void sscal_(n, sa, sx, incx)
integer *n;
real *sa, *sx;
integer *incx;
{
    /* Local variables */
    static integer i, m, nincx;

/*     scales a vector by a constant.					*/
/*     uses unrolled loops for increment equal to 1.			*/
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
        sx[i] = *sa * sx[i];
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
        sx[i] = *sa * sx[i];
    }
    if (*n < 5) {
        return;
    }
L40:
    for (i = m; i < *n; i += 5) {
        sx[i] = *sa * sx[i];
        sx[i+1] = *sa * sx[i+1];
        sx[i+2] = *sa * sx[i+2];
        sx[i+3] = *sa * sx[i+3];
        sx[i+4] = *sa * sx[i+4];
    }
    return;
} /* sscal_ */

