/* datapac/dnorcdf.f -- translated by f2c (version 20050501).
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

/*<       SUBROUTINE DNORCDF(X,CDF) >*/
/* Subroutine */ int dnorcdf_(doublereal *x, doublereal *cdf)
{
    /* Initialized data */

    static doublereal b1 = .31938153;
    static doublereal b2 = -.356563782;
    static doublereal b3 = 1.781477937;
    static doublereal b4 = -15978.;
    static doublereal b5 = 1.330274429;
    static doublereal p = .2316419;

    /* System generated locals */
    doublereal d__1, d__2, d__3, d__4, d__5;

    /* Builtin functions */
    double exp(doublereal);

    /* Local variables */
    doublereal t, z__;
    integer ipr;


/*     PURPOSE--THIS SUBROUTINE COMPUTES THE CUMULATIVE DISTRIBUTION */
/*              FUNCTION VALUE FOR THE NORMAL (GAUSSIAN) */
/*              DISTRIBUTION WITH MEAN = 0 AND STANDARD DEVIATION = 1. */
/*              THIS DISTRIBUTION IS DEFINED FOR ALL X AND HAS */
/*              THE PROBABILITY DENSITY FUNCTION */
/*              F(X) = (1/SQRT(2*PI))*EXP(-X*X/2). */
/*     INPUT  ARGUMENTS--X      = THE SINGLE PRECISION VALUE AT */
/*                                WHICH THE CUMULATIVE DISTRIBUTION */
/*                                FUNCTION IS TO BE EVALUATED. */
/*     OUTPUT ARGUMENTS--CDF    = THE SINGLE PRECISION CUMULATIVE */
/*                                DISTRIBUTION FUNCTION VALUE. */
/*     OUTPUT--THE SINGLE PRECISION CUMULATIVE DISTRIBUTION */
/*             FUNCTION VALUE CDF. */
/*     PRINTING--NONE. */
/*     RESTRICTIONS--NONE. */
/*     OTHER DATAPAC   SUBROUTINES NEEDED--NONE. */
/*     FORTRAN LIBRARY SUBROUTINES NEEDED--EXP. */
/*     MODE OF INTERNAL OPERATIONS--SINGLE PRECISION. */
/*     LANGUAGE--ANSI FORTRAN. */
/*     REFERENCES--NATIONAL BUREAU OF STANDARDS APPLIED MATHEMATICS */
/*                 SERIES 55, 1964, PAGE 932, FORMULA 26.2.17. */
/*               --JOHNSON AND KOTZ, CONTINUOUS UNIVARIATE */
/*                 DISTRIBUTIONS--1, 1970, PAGES 40-111. */
/*     WRITTEN BY--JAMES J. FILLIBEN */
/*                 STATISTICAL ENGINEERING LABORATORY (205.03) */
/*                 NATIONAL BUREAU OF STANDARDS */
/*                 WASHINGTON, D. C. 20234 */
/*                 PHONE:  301-921-2315 */
/*     ORIGINAL VERSION--JUNE      1972. */
/*     UPDATED         --SEPTEMBER 1975. */
/*     UPDATED         --NOVEMBER  1975. */

/* --------------------------------------------------------------------- */

/*<       DOUBLE PRECISION X,CDF >*/
/*<       INTEGER IPR >*/
/*<       DOUBLE PRECISION Z >*/
/*<       DOUBLE PRECISION T >*/
/*<    >*/

/*<       IPR=6 >*/
    ipr = 6;

/*     CHECK THE INPUT ARGUMENTS FOR ERRORS. */
/*     NO INPUT ARGUMENT ERRORS POSSIBLE */
/*     FOR THIS DISTRIBUTION. */

/* -----START POINT----------------------------------------------------- */

/*<       Z=X  >*/
    z__ = *x;
/*<       IF(X.LT.0.0)Z=-Z >*/
    if (*x < 0.) {
	z__ = -z__;
    }
/*<       T=1.0/(1.0+P*Z) >*/
    t = 1. / (p * z__ + 1.);
/*<    >*/
/* Computing 2nd power */
    d__1 = t;
/* Computing 3rd power */
    d__2 = t;
/* Computing 4th power */
    d__3 = t, d__3 *= d__3;
/* Computing 5th power */
    d__4 = t, d__5 = d__4, d__4 *= d__4;
    *cdf = 1. - exp(z__ * -.5 * z__) * .39894228040143 * 
	    (b1 * t + b2 * (d__1 * d__1) + b3 * (d__2 * (d__2 * d__2)) + b4 * 
	    (d__3 * d__3) + b5 * (d__5 * (d__4 * d__4)));
/*<       IF(X.LT.0.0)CDF=1.0-CDF  >*/
    if (*x < 0.) {
	*cdf = 1. - *cdf;
    }

/*<       RETURN >*/
    return 0;
/*<       END  >*/
} /* dnorcdf_ */

#ifdef __cplusplus
	}
#endif
