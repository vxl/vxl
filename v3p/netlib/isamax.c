/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

integer isamax_(integer *n, real *sx, integer *incx)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    static real smax__;
    static integer i, ix;

/*     finds the index of element having max. absolute value.		*/
/*     jack dongarra, linpack, 3/11/78.					*/
/*     modified 3/93 to return if incx .le. 0.				*/
/*     modified 12/3/93, array(1) declarations changed to array(*)	*/

    if (*n < 1 || *incx <= 0) {
        return 0;
    }
    if (*n == 1) {
        return 1;
    }
    if (*incx == 1) {
        goto L20;
    }

/*        code for increment not equal to 1 */

    smax__ = abs(sx[0]);
    ix = *incx;
    for (i = 1; i < *n; ++i, ix += *incx)
        if (abs(sx[ix]) > smax__) {
            ret_val = i+1;
            smax__ = abs(sx[ix]);
        }
    return ret_val;

/*        code for increment equal to 1 */

L20:
    smax__ = abs(sx[0]);
    for (i = 1; i < *n; ++i)
        if (abs(sx[i]) > smax__) {
            ret_val = i+1;
            smax__ = abs(sx[i]);
        }
    return ret_val;
} /* isamax_ */

