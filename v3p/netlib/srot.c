/* srot.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Subroutine */ void srot_(n, sx, incx, sy, incy, c, s)
integer *n;
real *sx;
integer *incx;
real *sy;
integer *incy;
real *c, *s;
{
    /* Local variables */
    static integer i;
    static real stemp;
    static integer ix, iy;

/*     applies a plane rotation. */
/*     jack dongarra, linpack, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

    /* Function Body */
    if (*n <= 0) {
        return;
    }
    if (*incx == 1 && *incy == 1) {
        for (i = 0; i < *n; ++i) {
            stemp = *c * sx[i] + *s * sy[i];
            sy[i] = *c * sy[i] - *s * sx[i];
            sx[i] = stemp;
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
            stemp  = *c * sx[ix] + *s * sy[iy];
            sy[iy] = *c * sy[iy] - *s * sx[ix];
            sx[ix] = stemp;
            ix += *incx; iy += *incy;
        }
    }
} /* srot_ */

