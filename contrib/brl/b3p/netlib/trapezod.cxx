/* trapezod.f -- translated by f2c (version 20020621).
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

/*     Algorithm 7.1 (Composite Trapezoidal Rule). */
/*     Section 7.2, Composite Trapezoidal and Simpson's Rule, Page 365 */

/* Subroutine */ int trapru_(E_fp f, real *a, real *b, integer *m, real *
	trule)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static real h__;
    static integer k;
    static real x, sum;

    h__ = (*b - *a) / *m;
    sum = (float)0.;
    i__1 = *m - 1;
    for (k = 1; k <= i__1; ++k) {
	x = *a + h__ * k;
	sum += (*f)(&x);
    }
    sum = h__ * ((*f)(a) + (*f)(b) + sum * 2) / 2;
    *trule = sum;
    return 0;
} /* trapru_ */

/* Subroutine */ int xtrapru_(E_fp f, real *a, real *b, integer *m, real *
	trule)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static real h__;
    static integer k;
    static real x, sum;

/*     This subroutine uses labeled DO loop(s). */
    h__ = (*b - *a) / *m;
    sum = (float)0.;
    i__1 = *m - 1;
    for (k = 1; k <= i__1; ++k) {
	x = *a + h__ * k;
	sum += (*f)(&x);
/* L10: */
    }
    sum = h__ * ((*f)(a) + (*f)(b) + sum * 2) / 2;
    *trule = sum;
    return 0;
} /* xtrapru_ */

#ifdef __cplusplus
	}
#endif
