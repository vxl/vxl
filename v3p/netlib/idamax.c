/*  -- translated by f2c (version 19940927).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

integer idamax_(integer *n, doublereal *dx, integer *incx)
{
    /* System generated locals */
    integer ret_val;

    /* Local variables */
    static doublereal dmax__;
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

    dmax__ = abs(dx[0]);
    ix = *incx;
    for (i = 1; i < *n; ++i, ix += *incx)
        if (abs(dx[ix]) > dmax__) {
            ret_val = i+1;
            dmax__ = abs(dx[ix]);
        }
    return ret_val;

/*        code for increment equal to 1 */

L20:
    dmax__ = abs(dx[0]);
    for (i = 1; i < *n; ++i)
        if (abs(dx[i]) > dmax__) {
            ret_val = i+1;
            dmax__ = abs(dx[i]);
        }
    return ret_val;

} /* idamax_ */
