/*  -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */

doublereal scnrm2_(n, x, incx)
integer *n;
complex *x;
integer *incx;
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    double sqrt();

    /* Local variables */
    static real temp, norm, scale;
    static integer ix;
    static real ssq;

/*  SCNRM2 returns the euclidean norm of a vector via the function */
/*  name, so that                                                  */
/*                                                                 */
/*     SCNRM2 := sqrt( conjg( x' )*x )                             */


/*  -- This version written on 25-October-1982. */
/*     Modified on 14-October-1993 to inline the call to CLASSQ. */
/*     Sven Hammarling, Nag Ltd. */

    /* Function Body */

    if (*n < 1 || *incx < 1) {
        norm = 0.f;
    } else {
        scale = 0.f;
        ssq = 1.f;
/*        The following loop is equivalent to this call to the LAPACK */
/*        auxiliary routine: */
/*        CALL CLASSQ( N, X, INCX, SCALE, SSQ ) */

        i__1 = (*n - 1) * *incx;
        for (ix = 0; *incx < 0 ? ix >= i__1 : ix <= i__1; ix += *incx) {
            if (x[ix].r != 0.f) {
                temp = (float)dabs(x[ix].r);
                if (scale < temp) {
                    r__1 = scale / temp;
                    ssq = ssq * (r__1 * r__1) + 1.f;
                    scale = temp;
                } else {
                    r__1 = temp / scale;
                    ssq += r__1 * r__1;
                }
            }
            if (x[ix].i != 0.f) {
                temp = (float)dabs(x[ix].i);
                if (scale < temp) {
                    r__1 = scale / temp;
                    ssq = ssq * (r__1 * r__1) + 1.f;
                    scale = temp;
                } else {
                    r__1 = temp / scale;
                    ssq += r__1 * r__1;
                }
            }
        }
        norm = scale * (float)sqrt(ssq);
    }

    return norm;

/*     End of SCNRM2. */

} /* scnrm2_ */

