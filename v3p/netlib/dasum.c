/* dasum.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

doublereal dasum_(n, dx, incx)
integer *n;
doublereal *dx;
integer *incx;
{
    /* Local variables */
    static integer i, m;
    static doublereal dtemp;
    static integer nincx;

/*     takes the sum of the absolute values.				*/
/*     jack dongarra, linpack, 3/11/78.					*/
/*     modified 3/93 to return if incx .le. 0.				*/
/*     modified 12/3/93, array(1) declarations changed to array(*)	*/

    dtemp = 0.;
    if (*n <= 0 || *incx <= 0) {
        return 0.;
    }
    if (*incx == 1) {
        goto L20;
    }

/*        code for increment not equal to 1 */

    nincx = *n * *incx;
    for (i = 0; i < nincx; i += *incx) {
        dtemp += abs(dx[i]);
    }
    return dtemp;

/*        code for increment equal to 1 */

/*        clean-up loop */

L20:
    m = *n % 6;
    if (m == 0) {
        goto L40;
    }
    for (i = 0; i < m; ++i) {
        dtemp += abs(dx[i]);
    }
    if (*n < 6) {
        goto L60;
    }
L40:
    for (i = m; i < *n; i += 6) {
        dtemp += abs(dx[i]) + abs(dx[i+1]) + abs(dx[i+2]) + abs(dx[i+3]) + abs(dx[i+4]) + abs(dx[i+5]);
    }
L60:
    return dtemp;
} /* dasum_ */

