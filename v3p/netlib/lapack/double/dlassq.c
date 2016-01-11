/* lapack/double/dlassq.f -- translated by f2c (version 20050501).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "v3p_netlib.h"

/*<       SUBROUTINE DLASSQ( N, X, INCX, SCALE, SUMSQ ) >*/
/* Subroutine */ int dlassq_(integer *n, doublereal *x, integer *incx,
        doublereal *scale, doublereal *sumsq)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Local variables */
    integer ix;
    doublereal absxi;


/*  -- LAPACK auxiliary routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     June 30, 1999 */

/*     .. Scalar Arguments .. */
/*<       INTEGER            INCX, N >*/
/*<       DOUBLE PRECISION   SCALE, SUMSQ >*/
/*     .. */
/*     .. Array Arguments .. */
/*<       DOUBLE PRECISION   X( * ) >*/
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLASSQ  returns the values  scl  and  smsq  such that */

/*     ( scl**2 )*smsq = x( 1 )**2 +...+ x( n )**2 + ( scale**2 )*sumsq, */

/*  where  x( i ) = X( 1 + ( i - 1 )*INCX ). The value of  sumsq  is */
/*  assumed to be non-negative and  scl  returns the value */

/*     scl = max( scale, abs( x( i ) ) ). */

/*  scale and sumsq must be supplied in SCALE and SUMSQ and */
/*  scl and smsq are overwritten on SCALE and SUMSQ respectively. */

/*  The routine makes only one pass through the vector x. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The number of elements to be used from the vector X. */

/*  X       (input) DOUBLE PRECISION array, dimension (N) */
/*          The vector for which a scaled sum of squares is computed. */
/*             x( i )  = X( 1 + ( i - 1 )*INCX ), 1 <= i <= n. */

/*  INCX    (input) INTEGER */
/*          The increment between successive values of the vector X. */
/*          INCX > 0. */

/*  SCALE   (input/output) DOUBLE PRECISION */
/*          On entry, the value  scale  in the equation above. */
/*          On exit, SCALE is overwritten with  scl , the scaling factor */
/*          for the sum of squares. */

/*  SUMSQ   (input/output) DOUBLE PRECISION */
/*          On entry, the value  sumsq  in the equation above. */
/*          On exit, SUMSQ is overwritten with  smsq , the basic sum of */
/*          squares from which  scl  has been factored out. */

/* ===================================================================== */

/*     .. Parameters .. */
/*<       DOUBLE PRECISION   ZERO >*/
/*<       PARAMETER          ( ZERO = 0.0D+0 ) >*/
/*     .. */
/*     .. Local Scalars .. */
/*<       INTEGER            IX >*/
/*<       DOUBLE PRECISION   ABSXI >*/
/*     .. */
/*     .. Intrinsic Functions .. */
/*<       INTRINSIC          ABS >*/
/*     .. */
/*     .. Executable Statements .. */

/*<       IF( N.GT.0 ) THEN >*/
    /* Parameter adjustments */
    --x;

    /* Function Body */
    if (*n > 0) {
/*<          DO 10 IX = 1, 1 + ( N-1 )*INCX, INCX >*/
        i__1 = (*n - 1) * *incx + 1;
        i__2 = *incx;
        for (ix = 1; i__2 < 0 ? ix >= i__1 : ix <= i__1; ix += i__2) {
/*<             IF( X( IX ).NE.ZERO ) THEN >*/
            if (x[ix] != 0.) {
/*<                ABSXI = ABS( X( IX ) ) >*/
                absxi = (d__1 = x[ix], abs(d__1));
/*<                IF( SCALE.LT.ABSXI ) THEN >*/
                if (*scale < absxi) {
/*<                   SUMSQ = 1 + SUMSQ*( SCALE / ABSXI )**2 >*/
/* Computing 2nd power */
                    d__1 = *scale / absxi;
                    *sumsq = *sumsq * (d__1 * d__1) + 1;
/*<                   SCALE = ABSXI >*/
                    *scale = absxi;
/*<                ELSE >*/
                } else {
/*<                   SUMSQ = SUMSQ + ( ABSXI / SCALE )**2 >*/
/* Computing 2nd power */
                    d__1 = absxi / *scale;
                    *sumsq += d__1 * d__1;
/*<                END IF >*/
                }
/*<             END IF >*/
            }
/*<    10    CONTINUE >*/
/* L10: */
        }
/*<       END IF >*/
    }
/*<       RETURN >*/
    return 0;

/*     End of DLASSQ */

/*<       END >*/
} /* dlassq_ */

#ifdef __cplusplus
        }
#endif
