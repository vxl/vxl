/*  -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */

/* Subroutine */ void zlacgv_(n, x, incx)
integer *n;
doublecomplex *x;
integer *incx;
{
    /* Local variables */
    static integer ioff, i;


/*  -- LAPACK auxiliary routine (version 2.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     October 31, 1992 */

/* ===================================================================== */
/*                                                                       */
/*  Purpose                                                              */
/*  =======                                                              */
/*                                                                       */
/*  ZLACGV conjugates a complex vector of length N.                      */
/*                                                                       */
/*  Arguments                                                            */
/*  =========                                                            */
/*                                                                       */
/*  N       (input) INTEGER                                              */
/*          The length of the vector X.  N >= 0.                         */
/*                                                                       */
/*  X       (input/output) COMPLEX*16 array, dimension                   */
/*                         (1+(N-1)*abs(INCX))                           */
/*          On entry, the vector of length N to be conjugated.           */
/*          On exit, X is overwritten with conjg(X).                     */
/*                                                                       */
/*  INCX    (input) INTEGER                                              */
/*          The spacing between successive elements of X.                */
/*                                                                       */
/* ===================================================================== */

    /* Function Body */

    if (*incx == 1) {
        for (i = 0; i < *n; ++i) {
            x[i].i = -x[i].i; /* d_cnjg(&x[i], &x[i]); */
        }
    } else {
        ioff = 0;
        if (*incx < 0) {
            ioff = (1-(*n)) * *incx;
        }
        for (i = 0; i < *n; ++i) {
            x[ioff].i = -x[ioff].i; /* d_cnjg(&x[ioff], &x[ioff]); */
            ioff += *incx;
        }
    }

/*     End of ZLACGV */

} /* zlacgv_ */

