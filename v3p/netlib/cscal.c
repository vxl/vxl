/*  -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */


/* Subroutine */ void cscal_(n, ca, cx, incx)
integer *n;
complex *ca, *cx;
integer *incx;
{
    /* System generated locals */
    complex q__1;

    /* Local variables */
    static integer i;


/*     scales a vector by a constant. */
/*     jack dongarra, linpack,  3/11/78. */
/*     modified 3/93 to return if incx .le. 0. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

    /* Function Body */

    if (*n <= 0 || *incx <= 0) {
        return;
    }

    if (*incx == 1) {
        for (i = 0; i < *n; ++i) {
            q__1.r = ca->r * cx[i].r - ca->i * cx[i].i,
            q__1.i = ca->r * cx[i].i + ca->i * cx[i].r;
            cx[i].r = q__1.r, cx[i].i = q__1.i;
        }
    }
    else {
        for (i = 0; i < *n * *incx; i += *incx) {
            q__1.r = ca->r * cx[i].r - ca->i * cx[i].i,
            q__1.i = ca->r * cx[i].i + ca->i * cx[i].r;
            cx[i].r = q__1.r, cx[i].i = q__1.i;
        }
    }
    return;
} /* cscal_ */

