/* cdotc.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */

/* Complex */ void cdotc_( ret_val, n, cx, incx, cy, incy)
complex * ret_val;
integer *n;
complex *cx;
integer *incx;
complex *cy;
integer *incy;
{
    /* Builtin functions */
    void r_cnjg();

    /* Local variables */
    static integer i;
    static complex ctemp;
    static integer ix, iy;

/*     forms the dot product of two vectors, conjugating the first */
/*     vector. */
/*     jack dongarra, linpack,  3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

    /* Function Body */

    ctemp.r = 0.f, ctemp.i = 0.f;
    if (*n <= 0) {
        ret_val->r = 0.f, ret_val->i = 0.f;
        return;
    }
    if (*incx == 1 && *incy == 1) {
        for (i = 0; i < *n; ++i) {
            ctemp.r += cx[i].r * cy[i].r + cx[i].i * cy[i].i,
            ctemp.i += cx[i].r * cy[i].i - cx[i].i * cy[i].r;
        }
        ret_val->r = ctemp.r, ret_val->i = ctemp.i;
    }
    else
    {
        ix = 0; iy = 0;
        if (*incx < 0) {
            ix = (1-(*n)) * *incx;
        }
        if (*incy < 0) {
            iy = (1-(*n)) * *incy;
        }
        for (i = 0; i < *n; ++i) {
            ctemp.r += cx[ix].r * cy[iy].r + cx[ix].i * cy[iy].i,
            ctemp.i += cx[ix].r * cy[iy].i - cx[ix].i * cy[iy].r;
            ix += *incx; iy += *incy;
        }
        ret_val->r = ctemp.r, ret_val->i = ctemp.i;
    }
} /* cdotc_ */

