/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

integer idamax_(integer *n, doublereal *dx, integer *incx)
{
    /* System generated locals */
    integer ret_val = 1;

    /* Local variables */
    static doublereal dmax;
    static integer i, ix;

/*     finds the index of element having max. absolute value.           */
/*     jack dongarra, linpack, 3/11/78.                                 */
/*     modified 3/93 to return if incx .le. 0.                          */
/*     modified 12/3/93, array(1) declarations changed to array(*)      */

    if (*n < 1 || *incx <= 0) {
        return 0;
    }
    if (*n == 1) {
        return 1;
    }
    dmax = abs(dx[0]);
/*        code for increment equal to 1 */
    if (*incx == 1) {
        for (i = 1; i < *n; ++i)
            if (abs(dx[i]) > dmax) {
                ret_val = i+1;
                dmax = abs(dx[i]);
            }
    }
/*        code for increment not equal to 1 */
    else {
        ix = *incx;
        for (i = 1; i < *n; ++i, ix += *incx)
            if (abs(dx[ix]) > dmax) {
                ret_val = i+1;
                dmax = abs(dx[ix]);
            }
    }
    return ret_val;
} /* idamax_ */
