/* zdotc.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */
/*                               and moved out of zsvdc.c to separate file */


/* Double Complex */
void zdotc_( ret_val, n, zx, incx, zy, incy)
doublecomplex * ret_val;
integer *n;
doublecomplex *zx;
integer *incx;
doublecomplex *zy;
integer *incy;
{
    /* System generated locals */
    doublecomplex z__1;

    /* Builtin functions */
    void d_cnjg();

    /* Local variables */
    static integer i;
    static doublecomplex ztemp;
    static integer ix, iy;


/*     forms the dot product of a vector. */
/*     jack dongarra, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

    /* Function Body */

    if (*n <= 0) {
        ret_val->r = 0., ret_val->i = 0.;
        return;
    }
    ztemp.r = 0., ztemp.i = 0.;

    if (*incx == 1 && *incy == 1) {
        for (i = 0; i < *n; ++i) {
            d_cnjg(&z__1, &zx[i]);
            ztemp.r += z__1.r * zy[i].r - z__1.i * zy[i].i,
            ztemp.i += z__1.r * zy[i].i + z__1.i * zy[i].r;
        }
        ret_val->r = ztemp.r, ret_val->i = ztemp.i;
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
            d_cnjg(&z__1, &zx[ix]);
            ztemp.r += z__1.r * zy[iy].r - z__1.i * zy[iy].i,
            ztemp.i += z__1.r * zy[iy].i + z__1.i * zy[iy].r;
            ix += *incx; iy += *incy;
        }
        ret_val->r = ztemp.r, ret_val->i = ztemp.i;
    }
    return;

} /* zdotc_ */
