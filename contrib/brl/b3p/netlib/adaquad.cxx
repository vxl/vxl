/* adaquad.f -- translated by f2c (version 20020621).
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

/*     Algorithm 7.5 (Adaptive Quadrature Using Simpson's Rule). */
/*     Section 7.4, Adaptive Quadrature, Page 389 */

/*     add missing variable F in Refine subrutine. */
/* Subroutine */ int adaptquad_(E_fp f, real *a, real *b, real *tol, real *
	srmat, real *integral, real *errbdd, integer *m, integer *state)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Local variables */
    static integer j, k, n, iterating;
    static real sum1, sum2;
    static integer done;
    static real srvec[11];
    extern /* Subroutine */ int srule_(U_fp, real *, real *, real *, real *), 
	    refine_(U_fp, integer *, real *, integer *, integer *);

    /* Parameter adjustments */
    srmat -= 102;

    /* Function Body */
    iterating = 0;
    done = 1;
    srule_((U_fp)f, a, b, tol, srvec);
    for (k = 1; k <= 11; ++k) {
	srmat[k * 101 + 1] = srvec[k - 1];
    }
    *m = 1;
    *state = iterating;
    while(*state == iterating) {
	n = *m;
	for (j = n; j >= 1; --j) {
	    refine_((U_fp)f, &j, &srmat[102], m, state);
	}
    }
    sum1 = (float)0.;
    sum2 = (float)0.;
    i__1 = *m;
    for (j = 1; j <= i__1; ++j) {
	sum1 += srmat[j + 808];
	sum2 += (r__1 = srmat[j + 909], dabs(r__1));
    }
    *integral = sum1;
    *errbdd = sum2;
    return 0;
} /* adaptquad_ */

/* Subroutine */ int refine_(U_fp f, integer *p, real *srmat, integer *m, 
	integer *state)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Local variables */
    static real a, b, c__;
    static integer j, k;
    static real s;
    static integer iterating;
    static real s2, fa, fb, fc, err, tol, tol2;
    static integer done;
    static real check;
    extern /* Subroutine */ int srule_(U_fp, real *, real *, real *, real *);
    static real sr0vec[11], sr1vec[11], sr2vec[11];

    /* Parameter adjustments */
    srmat -= 102;

    /* Function Body */
    iterating = 0;
    done = 1;
    *state = done;
    for (k = 1; k <= 11; ++k) {
	sr0vec[k - 1] = srmat[*p + k * 101];
    }
    a = sr0vec[0];
    c__ = sr0vec[1];
    b = sr0vec[2];
    fa = sr0vec[3];
    fc = sr0vec[4];
    fb = sr0vec[5];
    s = sr0vec[6];
    s2 = sr0vec[7];
    err = sr0vec[8];
    tol = sr0vec[9];
    check = sr0vec[10];
    if (check == (float)1.) {
	return 0;
    }
    tol2 = tol / 2;
    srule_((U_fp)f, &a, &c__, &tol2, sr1vec);
    srule_((U_fp)f, &c__, &b, &tol2, sr2vec);
    err = (r__1 = sr0vec[6] - sr1vec[6] - sr2vec[6], dabs(r__1)) / 10;
    if (err < tol) {
	sr0vec[10] = (float)1.;
    }
    if (err < tol) {
	for (k = 1; k <= 11; ++k) {
	    srmat[*p + k * 101] = sr0vec[k - 1];
	}
	srmat[*p + 808] = sr1vec[6] + sr2vec[6];
	srmat[*p + 909] = err;
    } else {
	i__1 = *p;
	for (j = *m + 1; j >= i__1; --j) {
	    for (k = 1; k <= 11; ++k) {
		srmat[j + k * 101] = srmat[j - 1 + k * 101];
	    }
	}
	++(*m);
	for (k = 1; k <= 11; ++k) {
	    srmat[*p + k * 101] = sr1vec[k - 1];
	}
	for (k = 1; k <= 11; ++k) {
	    srmat[*p + 1 + k * 101] = sr2vec[k - 1];
	}
	*state = iterating;
    }
    return 0;
} /* refine_ */

/* Subroutine */ int srule_(E_fp f, real *a, real *b, real *tol0, real *srvec)
{
    static real c__, h__, s, s2, fa, fb, fc, err, tol1, check;

    /* Parameter adjustments */
    --srvec;

    /* Function Body */
    h__ = (*b - *a) / 2;
    c__ = (*a + *b) / 2;
    fa = (*f)(a);
    fc = (*f)(&c__);
    fb = (*f)(b);
    s = h__ * ((*f)(a) + (*f)(&c__) * 4 + (*f)(b)) / 3;
    s2 = s;
    tol1 = *tol0;
    err = *tol0;
    check = (float)0.;
    srvec[1] = *a;
    srvec[2] = c__;
    srvec[3] = *b;
    srvec[4] = fa;
    srvec[5] = fc;
    srvec[6] = fb;
    srvec[7] = s;
    srvec[8] = s2;
    srvec[9] = err;
    srvec[10] = tol1;
    srvec[11] = check;
    return 0;
} /* srule_ */

#ifdef __cplusplus
	}
#endif
