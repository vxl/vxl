/* snrm2.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

doublereal snrm2_(n, x, incx)
integer *n;
real *x;
integer *incx;
{
    /* System generated locals */
    real r__1;

    /* Builtin functions */
    double sqrt();
#define sqrtf(f) ((float)sqrt((double)(f)))

    /* Local variables */
    static real norm, scale, absxi;
    static integer ix;
    static real ssq;

/*  SNRM2 returns the euclidean norm of a vector via the function       */
/*  name, so that                                                       */
/*                                                                      */
/*     SNRM2 := sqrt( x'*x )                                            */
/*                                                                      */
/*  -- This version written on 25-October-1982.                         */
/*     Modified on 14-October-1993 to inline the call to SLASSQ.        */
/*     Sven Hammarling, Nag Ltd.                                        */

    if (*n < 1 || *incx < 1) {
        norm = 0.0f;
    } else if (*n == 1) {
        norm = (float)dabs(x[0]);
    } else {
        scale = 0.0f;
        ssq = 1.0f;
/*        The following loop is equivalent to this call to the LAPACK */
/*        auxiliary routine: */
/*        CALL SLASSQ( N, X, INCX, SCALE, SSQ ) */

        for (ix = 0; ix < *n * *incx; ix += *incx) {
            if (x[ix] != 0.0f) {
                absxi = (float)dabs(x[ix]);
                if (scale < absxi) {
                    r__1 = scale / absxi;
                    ssq = ssq * r__1 * r__1 + 1.0f;
                    scale = absxi;
                } else {
                    r__1 = absxi / scale;
                    ssq += r__1 * r__1;
                }
            }
        }
        norm = scale * sqrtf(ssq);
    }

    return norm;

} /* snrm2_ */
