/* drot.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"
#ifdef KR_headers
/* Subroutine */ void drot_(n, dx, incx, dy, incy, c, s)
integer *n;
doublereal *dx;
integer *incx;
doublereal *dy;
integer *incy;
doublereal *c, *s;
#else
void drot_(
integer *n,
doublereal *dx,
integer *incx,
doublereal *dy,
integer *incy,
doublereal *c, doublereal*s)
#endif
{
    /* Local variables */
    static integer i;
    static doublereal dtemp;
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
            dtemp = *c * dx[i] + *s * dy[i];
            dy[i] = *c * dy[i] - *s * dx[i];
            dx[i] = dtemp;
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
            dtemp  = *c * dx[ix] + *s * dy[iy];
            dy[iy] = *c * dy[iy] - *s * dx[ix];
            dx[ix] = dtemp;
            ix += *incx; iy += *incy;
        }
    }
} /* drot_ */

