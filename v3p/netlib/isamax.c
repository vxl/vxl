/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

integer isamax_(integer *n, real *sx, integer *incx)
{
    /* System generated locals */
    integer ret_val = 1;

    /* Local variables */
    static real smax;
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
    smax = abs(sx[0]);
/*        code for increment equal to 1 */
    if (*incx == 1) {
        for (i = 1; i < *n; ++i)
            if (abs(sx[i]) > smax) {
                ret_val = i+1;
                smax = abs(sx[i]);
            }
    }
/*        code for increment not equal to 1 */
    else {
        ix = *incx;
        for (i = 1; i < *n; ++i, ix += *incx)
            if (abs(sx[ix]) > smax) {
                ret_val = i+1;
                smax = abs(sx[ix]);
            }
    }
    return ret_val;
} /* isamax_ */
