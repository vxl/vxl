/* ddot.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"
#ifdef KR_headers
doublereal ddot_(n, dx, incx, dy, incy)
integer *n;
doublereal *dx;
integer *incx;
doublereal *dy;
integer *incy;
#else
doublereal ddot_(integer *n, doublereal *dx, integer *incx, doublereal *dy, integer *incy)
#endif
{
    /* Local variables */
    static integer i, m;
    static doublereal dtemp;
    static integer ix, iy;

/*     forms the dot product of two vectors. */
/*     uses unrolled loops for increments equal to one. */
/*     jack dongarra, linpack, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

    /* Function Body */
    dtemp = 0.;
    if (*n <= 0) {
        return dtemp;
    }
    if (*incx == 1 && *incy == 1) {
        m = *n % 5;
        for (i = 0; i < m; ++i) {
            dtemp += dx[i] * dy[i];
        }
        for (i = m; i < *n; i += 5) {
            dtemp += dx[i] * dy[i] +
                     dx[i + 1] * dy[i + 1] +
                     dx[i + 2] * dy[i + 2] +
                     dx[i + 3] * dy[i + 3] +
                     dx[i + 4] * dy[i + 4];
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
            dtemp += dx[ix] * dy[iy];
            ix += *incx;
            iy += *incy;
        }
    }
    return dtemp;
} /* ddot_ */

