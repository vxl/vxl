/* simpson.f -- translated by f2c (version 20020621).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#ifdef __cplusplus
extern "C" {
#endif
#include "f2c.h"

/*     NUMERICAL METHODS: FORTRAN Programs, (c) John H. Mathews 1994 */
/*     To accompany the text: */
/*     NUMERICAL METHODS for Mathematics, Science and Engineering, 2nd Ed, 1992 */
/*     Prentice Hall, Englewood Cliffs, New Jersey, 07632, U.S.A. */
/*     This free software is complements of the author. */

/*     Algorithm 7.2 (Composite Simpson Rule). */
/*     Section 7.2, Composite Trapezoidal and Simpson's Rule, Page 365 */

/*     comment added by Kongbin Kang */
/*     F: integrand function */
/*     A: lower integration limit */
/*     B: higher integration limit */
/*     M: number of intervals. Notice, the subintervals used is 2M */
/*     Srule: output parameter to store simpson rule result */
/* Subroutine */ int simpru_(E_fp f, real *a, real *b, integer *m, real *
	srule)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static real h__;
    static integer k;
    static real x, sum, sumodd, sumeven;

    h__ = (*b - *a) / (*m << 1);
    sumeven = (float)0.;
    i__1 = *m - 1;
    for (k = 1; k <= i__1; ++k) {
	x = *a + h__ * 2 * k;
	sumeven += (*f)(&x);
    }
    sumodd = (float)0.;
    i__1 = *m;
    for (k = 1; k <= i__1; ++k) {
	x = *a + h__ * ((k << 1) - 1);
	sumodd += (*f)(&x);
    }
    sum = h__ * ((*f)(a) + (*f)(b) + sumeven * 2 + sumodd * 4) / 3;
    *srule = sum;
    return 0;
} /* simpru_ */

/* Subroutine */ int xsimpru_(E_fp f, real *a, real *b, integer *m, real *
	srule)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static real h__;
    static integer k;
    static real x, sum, sumodd, sumeven;

/*     This subroutine uses labeled DO loop(s). */
    h__ = (*b - *a) / (*m << 1);
    sumeven = (float)0.;
    i__1 = *m - 1;
    for (k = 1; k <= i__1; ++k) {
	x = *a + h__ * 2 * k;
	sumeven += (*f)(&x);
/* L10: */
    }
    sumodd = (float)0.;
    i__1 = *m;
    for (k = 1; k <= i__1; ++k) {
	x = *a + h__ * ((k << 1) - 1);
	sumodd += (*f)(&x);
/* L20: */
    }
    sum = h__ * ((*f)(a) + (*f)(b) + sumeven * 2 + sumodd * 4) / 3;
    *srule = sum;
    return 0;
} /* xsimpru_ */

#ifdef __cplusplus
	}
#endif
