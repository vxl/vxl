/* dcopy.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"
#ifdef KR_headers
/* Subroutine */ void dcopy_(n, dx, incx, dy, incy)
integer *n;
doublereal *dx;
integer *incx;
doublereal *dy;
integer *incy;
#else
void dcopy_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy)
#endif
{
    /* Local variables */
    static integer i, ix, iy;

/*     copies a vector, x, to a vector, y. */
/*     uses unrolled loops for increments equal to one. */
/*     jack dongarra, linpack, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

    /* Function Body */
    if (*n <= 0) {
        return;
    }
    if (*incx == 1 && *incy == 1) {
        for (i = 0; i < *n; ++i) {
            dy[i] = dx[i];
        }
    }
    else {
        ix = 0; iy = 0;
        if (*incx < 0) {
            ix = (1-(*n)) * *incx;
        }
        if (*incy < 0) {
            iy = (1-(*n)) * *incy;
        }
        for (i = 0; i < *n; ++i) {
            dy[iy] = dx[ix];
            ix += *incx;
            iy += *incy;
        }
    }
    return;
} /* dcopy_ */

