/* zscal.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */
/*                               and moved out of zsvdc.c to separate file */


/* Subroutine */ void zscal_(n, za, zx, incx)
integer *n;
doublecomplex *za, *zx;
integer *incx;
{
    /* System generated locals */
    doublecomplex z__1;

    /* Local variables */
    static integer i, ix;


/*     scales a vector by a constant. */
/*     jack dongarra, 3/11/78. */
/*     modified 3/93 to return if incx .le. 0. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */


    /* Function Body */
    if (*n <= 0 || *incx <= 0) {
        return;
    }
    if (*incx == 1) {
        for (i = 0; i < *n; ++i) {
            z__1.r = za->r * zx[i].r - za->i * zx[i].i,
            z__1.i = za->r * zx[i].i + za->i * zx[i].r;
            zx[i].r = z__1.r, zx[i].i = z__1.i;
        }
    }
    else {
        ix = 0;
        for (i = 0; i < *n; ++i) {
            z__1.r = za->r * zx[ix].r - za->i * zx[ix].i,
            z__1.i = za->r * zx[ix].i + za->i * zx[ix].r;
            zx[ix].r = z__1.r, zx[ix].i = z__1.i;
            ix += *incx;
        }
    }
} /* zscal_ */
