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

/*     takes the sum of the absolute values.                            */
/*     uses unrolled loops for increment equal to one.                  */
/*     jack dongarra, linpack, 3/11/78.                                 */
/*     modified 3/93 to return if incx .le. 0.                          */
/*     modified 12/3/93, array(1) declarations changed to array(*)      */

    if (*n <= 0 || *incx <= 0) {
        return 0.0f;
    }
    stemp = 0.0f;
/*        code for increment equal to 1 */
    if (*incx == 1) {
        m = *n % 6;
        for (i = 0; i < m; ++i) {
            stemp += (float)dabs(sx[i]);
        }
        for (i = m; i < *n; i += 6) {
            stemp += (float)(dabs(sx[i]) + dabs(sx[i+1]) + dabs(sx[i+2]) + dabs(sx[i+3]) + dabs(sx[i+4]) + dabs(sx[i+5]));
        }
    }
/*        code for increment not equal to 1 */
    else {
        nincx = *n * *incx;
        for (i = 0; i < nincx; i += *incx) {
            stemp += (float)dabs(sx[i]);
        }
    }
    return stemp;
} /* sasum_ */
