/* dqrdc.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Table of constant values */

static integer c__1 = 1;

/* Subroutine */ void dqrdc_(x, ldx, n, p, qraux, jpvt, work, job)
doublereal *x;
integer *ldx, *n, *p;
doublereal *qraux;
integer *jpvt;
doublereal *work;
integer *job;
{
    /* System generated locals */
    integer x_dim1, x_offset, i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_sign(), sqrt();

    /* Local variables */
    static logical negj;
    extern doublereal ddot_();
    static integer maxj;
    extern doublereal dnrm2_();
    static integer j, l;
    static doublereal t;
    extern /* Subroutine */ void dscal_(), dswap_();
    static logical swapj;
    extern /* Subroutine */ void daxpy_();
    static doublereal nrmxl;
    static integer jp, pl, pu;
    static doublereal tt, maxnrm;
    static integer lp1, lup;


/*     dqrdc uses householder transformations to compute the qr */
/*     factorization of an n by p matrix x.  column pivoting */
/*     based on the 2-norms of the reduced columns may be */
/*     performed at the users option. */

/*     on entry */

/*        x       double precision(ldx,p), where ldx .ge. n. */
/*                x contains the matrix whose decomposition is to be */
/*                computed. */

/*        ldx     integer. */
/*                ldx is the leading dimension of the array x. */

/*        n       integer. */
/*                n is the number of rows of the matrix x. */

/*        p       integer. */
/*                p is the number of columns of the matrix x. */

/*        jpvt    integer(p). */
/*                jpvt contains integers that control the selection */
/*                of the pivot columns.  the k-th column x(k) of x */
/*                is placed in one of three classes according to the */
/*                value of jpvt(k). */

/*                   if jpvt(k) .gt. 0, then x(k) is an initial */
/*                                      column. */

/*                   if jpvt(k) .eq. 0, then x(k) is a free column. */

/*                   if jpvt(k) .lt. 0, then x(k) is a final column. */

/*                before the decomposition is computed, initial columns */
/*                are moved to the beginning of the array x and final */
/*                columns to the end.  both initial and final columns */
/*                are frozen in place during the computation and only */
/*                free columns are moved.  at the k-th stage of the */
/*                reduction, if x(k) is occupied by a free column */
/*                it is interchanged with the free column of largest */
/*                reduced norm.  jpvt is not referenced if */
/*                job .eq. 0. */

/*        work    double precision(p). */
/*                work is a work array.  work is not referenced if */
/*                job .eq. 0. */

/*        job     integer. */
/*                job is an integer that initiates column pivoting. */
/*                if job .eq. 0, no pivoting is done. */
/*                if job .ne. 0, pivoting is done. */

/*     on return */

/*        x       x contains in its upper triangle the upper */
/*                triangular matrix r of the qr factorization. */
/*                below its diagonal x contains information from */
/*                which the orthogonal part of the decomposition */
/*                can be recovered.  note that if pivoting has */
/*                been requested, the decomposition is not that */
/*                of the original matrix x but that of x */
/*                with its columns permuted as described by jpvt. */

/*        qraux   double precision(p). */
/*                qraux contains further information required to recover */
/*                the orthogonal part of the decomposition. */

/*        jpvt    jpvt(k) contains the index of the column of the */
/*                original matrix that has been interchanged into */
/*                the k-th column, if pivoting was requested. */

/*     linpack. this version dated 08/14/78 . */
/*     g.w. stewart, university of maryland, argonne national lab. */

/*     dqrdc uses the following functions and subprograms. */

/*     blas daxpy,ddot,dscal,dswap,dnrm2 */
/*     fortran dabs,dmax1,min0,dsqrt */

/*     internal variables */



    /* Parameter adjustments */
    --work;
    --jpvt;
    --qraux;
    x_dim1 = *ldx;
    x_offset = x_dim1 + 1;
    x -= x_offset;

    /* Function Body */
    pl = 1;
    pu = 0;
    if (*job == 0) {
        goto L60;
    }

/*        pivoting has been requested.  rearrange the columns */
/*        according to jpvt. */

    for (j = 1; j <= *p; ++j) {
        swapj = jpvt[j] > 0;
        negj = jpvt[j] < 0;
        jpvt[j] = j;
        if (negj) {
            jpvt[j] = -j;
        }
        if (! swapj) {
            goto L10;
        }
        if (j != pl) {
            dswap_(n, &x[pl * x_dim1 + 1], &c__1, &x[j * x_dim1 + 1], &c__1);
        }
        jpvt[j] = jpvt[pl];
        jpvt[pl] = j;
        ++pl;
L10:
        ;
    }
    pu = *p;
    for (j = *p; j >= 1; --j) {
        if (jpvt[j] >= 0) {
            goto L40;
        }
        jpvt[j] = -jpvt[j];
        if (j == pu) {
            goto L30;
        }
        dswap_(n, &x[pu * x_dim1 + 1], &c__1, &x[j * x_dim1 + 1], &c__1);
        jp = jpvt[pu];
        jpvt[pu] = jpvt[j];
        jpvt[j] = jp;
L30:
        --pu;
L40:
        ;
    }
L60:

/*     compute the norms of the free columns. */

    if (pu < pl) {
        goto L80;
    }
    for (j = pl; j <= pu; ++j) {
        qraux[j] = dnrm2_(n, &x[j * x_dim1 + 1], &c__1);
        work[j] = qraux[j];
    }
L80:

/*     perform the householder reduction of x. */

    lup = min(*n,*p);
    for (l = 1; l <= lup; ++l) {
        if (l < pl || l >= pu) {
            goto L120;
        }

/*           locate the column of largest norm and bring it */
/*           into the pivot position. */

        maxnrm = 0.;
        maxj = l;
        for (j = l; j <= pu; ++j) {
            if (qraux[j] <= maxnrm) {
                goto L90;
            }
            maxnrm = qraux[j];
            maxj = j;
L90:
            ;
        }
        if (maxj == l) {
            goto L110;
        }
        dswap_(n, &x[l * x_dim1 + 1], &c__1, &x[maxj * x_dim1 + 1], &c__1);
        qraux[maxj] = qraux[l];
        work[maxj] = work[l];
        jp = jpvt[maxj];
        jpvt[maxj] = jpvt[l];
        jpvt[l] = jp;
L110:
L120:
        qraux[l] = 0.;
        if (l == *n) {
            goto L190;
        }

/*           compute the householder transformation for column l. */

        i__1 = *n - l + 1;
        nrmxl = dnrm2_(&i__1, &x[l + l * x_dim1], &c__1);
        if (nrmxl == 0.) {
            goto L180;
        }
        if (x[l + l * x_dim1] != 0.) {
            nrmxl = d_sign(&nrmxl, &x[l + l * x_dim1]);
        }
        i__1 = *n - l + 1;
        d__1 = 1. / nrmxl;
        dscal_(&i__1, &d__1, &x[l + l * x_dim1], &c__1);
        x[l + l * x_dim1] += 1.;

/*              apply the transformation to the remaining columns, */
/*              updating the norms. */

        lp1 = l + 1;
        if (*p < lp1) {
            goto L170;
        }
        for (j = lp1; j <= *p; ++j) {
            i__1 = *n - l + 1;
            t = -ddot_(&i__1, &x[l + l * x_dim1], &c__1, &x[l + j * x_dim1], &c__1) / x[l + l * x_dim1];
            daxpy_(&i__1, &t, &x[l + l * x_dim1], &c__1, &x[l + j * x_dim1], &c__1);
            if (j < pl || j > pu) {
                goto L150;
            }
            if (qraux[j] == 0.) {
                goto L150;
            }
            d__1 = abs(x[l + j * x_dim1]) / qraux[j];
            tt = 1. - d__1 * d__1;
            tt = max(tt,0.);
            t = tt;
            d__1 = qraux[j] / work[j];
            tt = tt * .05 * d__1 * d__1 + 1.;
            if (tt == 1.) {
                goto L130;
            }
            qraux[j] *= sqrt(t);
            goto L140;
L130:
            i__1 = *n - l;
            qraux[j] = dnrm2_(&i__1, &x[l + 1 + j * x_dim1], &c__1);
            work[j] = qraux[j];
L140:
L150:
            ;
        }
L170:

/*              save the transformation. */

        qraux[l] = x[l + l * x_dim1];
        x[l + l * x_dim1] = -nrmxl;
L180:
L190:
        ;
    }
    return;
} /* dqrdc_ */

