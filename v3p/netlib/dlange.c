/* dlange.f -- translated by f2c (version of 4 June 1993  1:43:59).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

doublereal dlange_(char *norm, integer *m, integer *n, doublereal *a, integer
        *lda, doublereal *work, ftnlen norm_len)
{
    /* System generated locals */
    integer a_dim1, a_offset;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    static integer i, j;
    static doublereal scale;
    extern logical lsame_(char *, char *, ftnlen, ftnlen);
    static doublereal value;
    extern /* Subroutine */ void dlassq_(integer *, doublereal *, integer *,
            doublereal *, doublereal *);
    static doublereal sum;


/*  -- LAPACK auxiliary routine (version 2.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     October 31, 1992 */

/*  Purpose */
/*  ======= */

/*  DLANGE  returns the value of the one norm,  or the Frobenius norm, or */
/*  the  infinity norm,  or the  element of  largest absolute value  of a */
/*  real matrix A. */

/*  Description */
/*  =========== */

/*  DLANGE returns the value */

/*     DLANGE = ( max(abs(A(i,j))), NORM = 'M' or 'm' */
/*              ( */
/*              ( norm1(A),         NORM = '1', 'O' or 'o' */
/*              ( */
/*              ( normI(A),         NORM = 'I' or 'i' */
/*              ( */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e' */

/*  where  norm1  denotes the  one norm of a matrix (maximum column sum), */
/*  normI  denotes the  infinity norm  of a matrix  (maximum row sum) and */
/*  normF  denotes the  Frobenius norm of a matrix (square root of sum of */
/*  squares).  Note that  max(abs(A(i,j)))  is not a  matrix norm. */

/*  Arguments */
/*  ========= */

/*  NORM    (input) CHARACTER*1 */
/*          Specifies the value to be returned in DLANGE as described */
/*          above. */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix A.  M >= 0.  When M = 0, */
/*          DLANGE is set to zero. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix A.  N >= 0.  When N = 0, */
/*          DLANGE is set to zero. */

/*  A       (input) DOUBLE PRECISION array, dimension (LDA,N) */
/*          The m by n matrix A. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(M,1). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension (LWORK), */
/*          where LWORK >= M when NORM = 'I'; otherwise, WORK is not */
/*          referenced. */

/* ===================================================================== */

    /* Parameter adjustments */
    --work;
    a_dim1 = *lda;
    a_offset = a_dim1 + 1;
    a -= a_offset;

    if (min(*m,*n) == 0) {
        value = 0.;
    } else if (lsame_(norm, "M", 1L, 1L)) {

/*        Find max(abs(A(i,j))). */

        value = 0.;
        for (j = 1; j <= *n; ++j) {
            for (i = 1; i <= *m; ++i) {
                value = max(value, abs(a[i + j * a_dim1]));
            }
        }
    } else if (lsame_(norm, "O", 1L, 1L) || *norm == '1') {

/*        Find norm1(A). */

        value = 0.;
        for (j = 1; j <= *n; ++j) {
            sum = 0.;
            for (i = 1; i <= *m; ++i) {
                sum += abs(a[i + j * a_dim1]);
            }
            value = max(value,sum);
        }
    } else if (lsame_(norm, "I", 1L, 1L)) {

/*        Find normI(A). */

        for (i = 1; i <= *m; ++i) {
            work[i] = 0.;
        }
        for (j = 1; j <= *n; ++j) {
            for (i = 1; i <= *m; ++i) {
                work[i] += abs(a[i + j * a_dim1]);
            }
        }
        value = 0.;
        for (i = 1; i <= *m; ++i) {
            value = max(value,work[i]);
        }
    } else if (lsame_(norm, "F", 1L, 1L) || lsame_(norm, "E", 1L, 1L)) {

/*        Find normF(A). */

        scale = 0.;
        sum = 1.;
        for (j = 1; j <= *n; ++j) {
            dlassq_(m, &a[j * a_dim1 + 1], &c__1, &scale, &sum);
        }
        value = scale * sqrt(sum);
    }

    return value;

} /* dlange_ */
