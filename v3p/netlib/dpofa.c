/* dpofa.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ void dpofa_(a, lda, n, info)
doublereal *a;
integer *lda, *n, *info;
{
    /* Builtin functions */
    double sqrt();

    /* Local variables */
    extern doublereal ddot_();
    static integer j, k;
    static doublereal s, t;

/*     dpofa factors a double precision symmetric positive definite	*/
/*     matrix.								*/
/*									*/
/*     dpofa is usually called by dpoco, but it can be called		*/
/*     directly with a saving in time if  rcond  is not needed.		*/
/*     (time for dpoco) = (1 + 18/n)*(time for dpofa) .			*/
/*									*/
/*     on entry								*/
/*									*/
/*        a       double precision(lda, n)				*/
/*                the symmetric matrix to be factored.  only the	*/
/*                diagonal and upper triangle are used.			*/
/*									*/
/*        lda     integer						*/
/*                the leading dimension of the array  a .		*/
/*									*/
/*        n       integer						*/
/*                the order of the matrix  a .				*/
/*									*/
/*     on return							*/
/*									*/
/*        a       an upper triangular matrix  r  so that a = trans(r)*r */
/*                where  trans(r)  is the transpose.			*/
/*                the strict lower triangle is unaltered.		*/
/*                if  info .ne. 0 , the factorization is not complete.	*/
/*									*/
/*        info    integer						*/
/*                = 0  for normal return.				*/
/*                = k  signals an error condition.  the leading minor	*/
/*                     of order  k  is not positive definite.		*/
/*									*/
/*     linpack.  this version dated 08/14/78 .				*/
/*     cleve moler, university of new mexico, argonne national lab.	*/
/*									*/
/*     subroutines and functions					*/
/*									*/
/*     blas ddot							*/
/*     fortran dsqrt							*/

    for (j = 0; j < *n; ++j) {
        *info = j+1;
        s = 0.;
        if (j < 1) {
            goto L20;
        }
        for (k = 0; k < j; ++k) {
            t = a[k + j * *lda] - ddot_(&k, &a[k * *lda], &c__1, &a[j * *lda], &c__1);
            t /= a[k + k * *lda];
            a[k + j * *lda] = t;
            s += t * t;
        }
L20:
        s = a[j + j * *lda] - s;
        if (s <= 0.) {
            return;
        }
        a[j + j * *lda] = sqrt(s);
    }
    *info = 0;
    return;
} /* dpofa_ */

