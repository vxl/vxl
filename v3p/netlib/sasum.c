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
    /* Local variables */
    static integer i, m, nincx;
    static real stemp;

/*     takes the sum of the absolute values.				*/
/*     uses unrolled loops for increment equal to one.			*/
/*     jack dongarra, linpack, 3/11/78.					*/
/*     modified 3/93 to return if incx .le. 0.				*/
/*     modified 12/3/93, array(1) declarations changed to array(*)	*/

    if (*n <= 0 || *incx <= 0) {
        return 0.0f;
    }
    stemp = 0.0f;
    if (*incx == 1) {
        goto L20;
    }

/*        code for increment not equal to 1 */

    nincx = *n * *incx;
    for (i = 0; i < nincx; i += *incx) {
        stemp += dabs(sx[i]);
    }
    return stemp;

/*        code for increment equal to 1 */


/*        clean-up loop */

L20:
    m = *n % 6;
    if (m == 0) {
        goto L40;
    }
    for (i = 0; i < m; ++i) {
        stemp += dabs(sx[i]);
    }
    if (*n < 6) {
        goto L60;
    }
L40:
    for (i = m; i < *n; i += 6) {
        stemp += dabs(sx[i]) + dabs(sx[i+1]) + dabs(sx[i+2]) + dabs(sx[i+3]) + dabs(sx[i+4]) + dabs(sx[i+5]);
    }
L60:
    return stemp;
} /* sasum_ */

