/* slange.f -- translated by f2c (version of 4 June 1993  1:43:59).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

real slange_(char *norm, integer *m, integer *n, real *a, integer *lda, real *work, ftnlen norm_len)
{
    /* Builtin functions */
    double sqrt(doublereal);
#define sqrtf(f) ((float)sqrt((double)(f)))

    /* Local variables */
    static integer i, j;
    static real scale;
    extern logical lsame_(char *, char *, ftnlen, ftnlen);
    static real value;
    extern /* Subroutine */ void slassq_(integer *, real *, integer *, real *, real *);
    static real sum;

/*  -- LAPACK auxiliary routine (version 2.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     October 31, 1992 */

/*  Purpose                                                              */
/*  =======                                                              */
/*                                                                       */
/*  SLANGE  returns the value of the one norm, or the Frobenius norm, or */
/*  the infinity norm, or the element of largest absolute value of a     */
/*  real matrix A.                                                       */
/*                                                                       */
/*  Description                                                          */
/*  ===========                                                          */
/*                                                                       */
/*  SLANGE returns the value                                             */
/*                                                                       */
/*     SLANGE = ( max(abs(A(i,j))), NORM = 'M' or 'm'                    */
/*              (                                                        */
/*              ( norm1(A),         NORM = '1', 'O' or 'o'               */
/*              (                                                        */
/*              ( normI(A),         NORM = 'I' or 'i'                    */
/*              (                                                        */
/*              ( normF(A),         NORM = 'F', 'f', 'E' or 'e'          */
/*                                                                       */
/*  where norm1 denotes the one norm of a matrix (maximum column sum),   */
/*  normI denotes the infinity norm of a matrix (maximum row sum) and    */
/*  normF denotes the Frobenius norm of a matrix (square root of sum of  */
/*  squares).  Note that  max(abs(A(i,j)))  is not a matrix norm.        */
/*                                                                       */
/*  Arguments                                                            */
/*  =========                                                            */
/*                                                                       */
/*  NORM    (input) CHARACTER*1                                          */
/*          Specifies the value to be returned in SLANGE as described    */
/*          above.                                                       */
/*                                                                       */
/*  M       (input) INTEGER                                              */
/*          The number of rows of the matrix A.  M >= 0.  When M = 0,    */
/*          SLANGE is set to zero.                                       */
/*                                                                       */
/*  N       (input) INTEGER                                              */
/*          The number of columns of the matrix A.  N >= 0.  When N = 0, */
/*          SLANGE is set to zero.                                       */
/*                                                                       */
/*  A       (input) REAL array, dimension (LDA,N)                        */
/*          The m by n matrix A.                                         */
/*                                                                       */
/*  LDA     (input) INTEGER                                              */
/*          The leading dimension of the array A.  LDA >= max(M,1).      */
/*                                                                       */
/*  WORK    (workspace) REAL array, dimension (LWORK),                   */
/*          where LWORK >= M when NORM = 'I'; otherwise, WORK is not     */
/*          referenced.                                                  */
/*                                                                       */
/* ===================================================================== */

    if (min(*m,*n) == 0) {
        return 0.f;
    } else if (lsame_(norm, "M", 1L, 1L)) {

/*        Find max(abs(A(i,j))). */

        value = 0.f;
        for (j = 0; j < *n; ++j) {
            for (i = 0; i < *m; ++i) {
                value = max(value, abs(a[i + j * *lda]));
            }
        }
    } else if (lsame_(norm, "O", 1L, 1L) || *norm == '1') {

/*        Find norm1(A). */

        value = 0.f;
        for (j = 0; j < *n; ++j) {
            sum = 0.f;
            for (i = 0; i < *m; ++i) {
                sum += abs(a[i + j * *lda]);
            }
            value = max(value,sum);
        }
    } else if (lsame_(norm, "I", 1L, 1L)) {

/*        Find normI(A). */

        for (i = 0; i < *m; ++i) {
            work[i] = 0.f;
        }
        for (j = 0; j < *n; ++j) {
            for (i = 0; i < *m; ++i) {
                work[i] += abs(a[i + j * *lda]);
            }
        }
        value = 0.f;
        for (i = 0; i < *m; ++i) {
            value = max(value,work[i]);
        }
    } else if (lsame_(norm, "F", 1L, 1L) || lsame_(norm, "E", 1L, 1L)) {

/*        Find normF(A). */

        scale = 0.f;
        sum = 1.f;
        for (j = 0; j < *n; ++j) {
            slassq_(m, &a[j * *lda], &c__1, &scale, &sum);
        }
        value = scale * sqrtf(sum);
    }

    return value;

} /* slange_ */
