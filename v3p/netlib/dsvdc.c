/* dsvdc.f -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/*
 * Calling this ensures that the operands are spilled to
 * memory and thus avoids excessive precision when compiling
 * for x86 with heavy optimization (gcc). It is better to do
 * this than to turn on -ffloat-store.
 */
static int fsm_ieee_doubles_equal(double *x, double *y);

/* Table of constant values */
static integer c__1 = 1;
static doublereal c_b44 = -1.;

/* Subroutine */ void dsvdc_(x, ldx, n, p, s, e, u, ldu, v, ldv, work, job, info)
doublereal *x;
integer *ldx, *n, *p;
doublereal *s, *e, *u;
integer *ldu;
doublereal *v;
integer *ldv;
doublereal *work;
integer *job, *info;
{
    /* System generated locals */
    integer x_dim1, x_offset, u_dim1, u_offset, v_dim1, v_offset, i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_sign(), sqrt();

    /* Local variables */
    static integer kase;
    extern doublereal ddot_();
    static integer jobu, iter;
    extern /* Subroutine */ void drot_();
    static doublereal test;
    extern doublereal dnrm2_();
    static integer nctp1;
    static doublereal b, c;
    static integer nrtp1;
    static doublereal f, g;
    static integer i, j, k, l, m;
    static doublereal t, scale;
    extern /* Subroutine */ void dscal_();
    static doublereal shift;
    extern /* Subroutine */ void dswap_(), drotg_();
    static integer maxit;
    extern /* Subroutine */ void daxpy_();
    static logical wantu, wantv;
    static doublereal t1, ztest, el;
    static doublereal cs;
    static integer mm, ls;
    static doublereal sl;
    static integer lu;
    static doublereal sm, sn;
    static integer lm1, lp1, nct, ncu, nrt;
    static doublereal emm1, smm1;

/*     dsvdc is a subroutine to reduce a double precision nxp matrix x  */
/*     by orthogonal transformations u and v to diagonal form.  the     */
/*     diagonal elements s(i) are the singular values of x.  the        */
/*     columns of u are the corresponding left singular vectors,        */
/*     and the columns of v the right singular vectors.                 */
/*                                                                      */
/*     on entry                                                         */
/*                                                                      */
/*         x         double precision(ldx,p), where ldx.ge.n.           */
/*                   x contains the matrix whose singular value         */
/*                   decomposition is to be computed.  x is             */
/*                   destroyed by dsvdc.                                */
/*                                                                      */
/*         ldx       integer.                                           */
/*                   ldx is the leading dimension of the array x.       */
/*                                                                      */
/*         n         integer.                                           */
/*                   n is the number of rows of the matrix x.           */
/*                                                                      */
/*         p         integer.                                           */
/*                   p is the number of columns of the matrix x.        */
/*                                                                      */
/*         ldu       integer.                                           */
/*                   ldu is the leading dimension of the array u.       */
/*                   (see below).                                       */
/*                                                                      */
/*         ldv       integer.                                           */
/*                   ldv is the leading dimension of the array v.       */
/*                   (see below).                                       */
/*                                                                      */
/*         work      double precision(n).                               */
/*                   work is a scratch array.                           */
/*                                                                      */
/*         job       integer.                                           */
/*                   job controls the computation of the singular       */
/*                   vectors.  it has the decimal expansion ab          */
/*                   with the following meaning                         */
/*                                                                      */
/*                        a.eq.0    do not compute the left singular    */
/*                                  vectors.                            */
/*                        a.eq.1    return the n left singular vectors  */
/*                                  in u.                               */
/*                        a.ge.2    return the first min(n,p) singular  */
/*                                  vectors in u.                       */
/*                        b.eq.0    do not compute the right singular   */
/*                                  vectors.                            */
/*                        b.eq.1    return the right singular vectors   */
/*                                  in v.                               */
/*                                                                      */
/*     on return                                                        */
/*                                                                      */
/*         s         double precision(mm), where mm=min(n+1,p).         */
/*                   the first min(n,p) entries of s contain the        */
/*                   singular values of x arranged in descending        */
/*                   order of magnitude.                                */
/*                                                                      */
/*         e         double precision(p),                               */
/*                   e ordinarily contains zeros.  however see the      */
/*                   discussion of info for exceptions.                 */
/*                                                                      */
/*         u         double precision(ldu,k), where ldu.ge.n.  if       */
/*                                   joba.eq.1 then k.eq.n, if joba.ge.2*/
/*                                   then k.eq.min(n,p).                */
/*                   u contains the matrix of left singular vectors.    */
/*                   u is not referenced if joba.eq.0.  if n.le.p       */
/*                   or if joba.eq.2, then u may be identified with x   */
/*                   in the subroutine call.                            */
/*                                                                      */
/*         v         double precision(ldv,p), where ldv.ge.p.           */
/*                   v contains the matrix of right singular vectors.   */
/*                   v is not referenced if job.eq.0.  if p.le.n,       */
/*                   then v may be identified with x in the             */
/*                   subroutine call.                                   */
/*                                                                      */
/*         info      integer.                                           */
/*                   the singular values (and their corresponding       */
/*                   singular vectors) s(info+1),s(info+2),...,s(m)     */
/*                   are correct (here m=min(n,p)).  thus if            */
/*                   info.eq.0, all the singular values and their       */
/*                   vectors are correct.  in any event, the matrix     */
/*                   b = trans(u)*x*v is the bidiagonal matrix          */
/*                   with the elements of s on its diagonal and the     */
/*                   elements of e on its super-diagonal (trans(u)      */
/*                   is the transpose of u).  thus the singular         */
/*                   values of x and b are the same.                    */
/*                                                                      */
/*     linpack. this version dated 08/14/78 .                           */
/*              correction made to shift 2/84.                          */
/*     g.w. stewart, university of maryland, argonne national lab.      */

/*     dsvdc uses the following functions and subprograms. */
/*                                                         */
/*     external drot                                       */
/*     blas daxpy,ddot,dscal,dswap,dnrm2,drotg             */
/*     fortran dabs,dmax1,max0,min0,mod,dsqrt              */

    /* Parameter adjustments */
    --work;
    v_dim1 = *ldv;
    v_offset = v_dim1 + 1;
    v -= v_offset;
    u_dim1 = *ldu;
    u_offset = u_dim1 + 1;
    u -= u_offset;
    --e;
    --s;
    x_dim1 = *ldx;
    x_offset = x_dim1 + 1;
    x -= x_offset;

/*     set the maximum number of iterations. */

    maxit = 30;

/*     determine what is to be computed. */

    wantu = FALSE_;
    wantv = FALSE_;
    jobu = *job % 100 / 10;
    ncu = *n;
    if (jobu > 1) {
        ncu = min(*n,*p);
    }
    if (jobu != 0) {
        wantu = TRUE_;
    }
    if (*job % 10 != 0) {
        wantv = TRUE_;
    }

/*     reduce x to bidiagonal form, storing the diagonal elements */
/*     in s and the super-diagonal elements in e. */

    *info = 0;
    nct = min(*n-1,*p);
    nrt = max(0,min(*p-2,*n));
    lu = max(nct,nrt);
    if (lu < 1) {
        goto L170;
    }
    for (l = 1; l <= lu; ++l) {
        lp1 = l + 1;
        if (l > nct) {
            goto L20;
        }

/*           compute the transformation for the l-th column and */
/*           place the l-th diagonal in s(l). */

        i__1 = *n - l + 1;
        s[l] = dnrm2_(&i__1, &x[l + l * x_dim1], &c__1);
        if (s[l] == 0.) {
            goto L10;
        }
        if (x[l + l * x_dim1] != 0.) {
            s[l] = d_sign(&s[l], &x[l + l * x_dim1]);
        }
        i__1 = *n - l + 1;
        d__1 = 1. / s[l];
        dscal_(&i__1, &d__1, &x[l + l * x_dim1], &c__1);
        x[l + l * x_dim1] += 1.;
L10:
        s[l] = -s[l];
L20:
        if (*p < lp1) {
            goto L50;
        }
        for (j = lp1; j <= *p; ++j) {
            if (l > nct) {
                goto L30;
            }
            if (s[l] == 0.) {
                goto L30;
            }

/*              apply the transformation. */

            i__1 = *n - l + 1;
            t = -ddot_(&i__1, &x[l + l * x_dim1], &c__1, &x[l + j * x_dim1], &c__1) / x[l + l * x_dim1];
            daxpy_(&i__1, &t, &x[l + l * x_dim1], &c__1, &x[l + j * x_dim1], &c__1);
L30:

/*           place the l-th row of x into  e for the */
/*           subsequent calculation of the row transformation. */

            e[j] = x[l + j * x_dim1];
        }
L50:
        if (! wantu || l > nct) {
            goto L70;
        }

/*           place the transformation in u for subsequent back */
/*           multiplication. */

        for (i = l; i <= *n; ++i) {
            u[i + l * u_dim1] = x[i + l * x_dim1];
        }
L70:
        if (l > nrt) {
            goto L150;
        }

/*           compute the l-th row transformation and place the */
/*           l-th super-diagonal in e(l). */

        i__1 = *p - l;
        e[l] = dnrm2_(&i__1, &e[lp1], &c__1);
        if (e[l] == 0.) {
            goto L80;
        }
        if (e[lp1] != 0.) {
            e[l] = d_sign(&e[l], &e[lp1]);
        }
        i__1 = *p - l;
        d__1 = 1. / e[l];
        dscal_(&i__1, &d__1, &e[lp1], &c__1);
        e[lp1] += 1.;
L80:
        e[l] = -e[l];
        if (lp1 > *n || e[l] == 0.) {
            goto L120;
        }

/*              apply the transformation. */

        for (i = lp1; i <= *n; ++i) {
            work[i] = 0.;
        }
        for (j = lp1; j <= *p; ++j) {
            i__1 = *n - l;
            daxpy_(&i__1, &e[j], &x[lp1 + j * x_dim1], &c__1, &work[lp1], &c__1);
        }
        for (j = lp1; j <= *p; ++j) {
            i__1 = *n - l;
            d__1 = -e[j] / e[lp1];
            daxpy_(&i__1, &d__1, &work[lp1], &c__1, &x[lp1 + j * x_dim1], &c__1);
        }
L120:
        if (! wantv) {
            goto L140;
        }

/*              place the transformation in v for subsequent */
/*              back multiplication. */

        for (i = lp1; i <= *p; ++i) {
            v[i + l * v_dim1] = e[i];
        }
L140:
L150:
        ;
    }
L170:

/*     set up the final bidiagonal matrix or order m. */

    m = min(*p,*n+1);
    nctp1 = nct + 1;
    nrtp1 = nrt + 1;
    if (nct < *p) {
        s[nctp1] = x[nctp1 + nctp1 * x_dim1];
    }
    if (*n < m) {
        s[m] = 0.;
    }
    if (nrtp1 < m) {
        e[nrtp1] = x[nrtp1 + m * x_dim1];
    }
    e[m] = 0.;

/*     if required, generate u. */

    if (wantu)
    for (j = nctp1; j <= ncu; ++j) {
        for (i = 1; i <= *n; ++i) {
            u[i + j * u_dim1] = 0.;
        }
        u[j + j * u_dim1] = 1.;
    }
    if (wantu)
    for (l = nct; l >= 1; --l) {
        if (s[l] == 0.) {
            for (i = 1; i <= *n; ++i) {
                u[i + l * u_dim1] = 0.;
            }
            u[l + l * u_dim1] = 1.;
            continue;
        }
        lp1 = l + 1;
        if (ncu < lp1) {
            goto L220;
        }
        for (j = lp1; j <= ncu; ++j) {
            i__1 = *n - l + 1;
            t = -ddot_(&i__1, &u[l + l * u_dim1], &c__1, &u[l + j * u_dim1], &c__1) / u[l + l * u_dim1];
            daxpy_(&i__1, &t, &u[l + l * u_dim1], &c__1, &u[l + j * u_dim1], &c__1);
        }
L220:
        i__1 = *n - l + 1;
        dscal_(&i__1, &c_b44, &u[l + l * u_dim1], &c__1);
        u[l + l * u_dim1] += 1.;
        lm1 = l - 1;
        for (i = 1; i <= lm1; ++i) {
            u[i + l * u_dim1] = 0.;
        }
    }

/*     if it is required, generate v. */

    if (wantv)
    for (l = *p; l >= 1; --l) {
        lp1 = l + 1;
        if (l > nrt) {
            goto L320;
        }
        if (e[l] == 0.) {
            goto L320;
        }
        for (j = lp1; j <= *p; ++j) {
            i__1 = *p - l;
            t = -ddot_(&i__1, &v[lp1 + l * v_dim1], &c__1, &v[lp1 + j * v_dim1], &c__1) / v[lp1 + l * v_dim1];
            daxpy_(&i__1, &t, &v[lp1 + l * v_dim1], &c__1, &v[lp1 + j * v_dim1], &c__1);
        }
L320:
        for (i = 1; i <= *p; ++i) {
            v[i + l * v_dim1] = 0.;
        }
        v[l + l * v_dim1] = 1.;
    }

/*     main iteration loop for the singular values. */

    mm = m;
    iter = 0;
L360:

/*        quit if all the singular values have been found. */

    if (m == 0) {
        return;
    }

/*        if too many iterations have been performed, set */
/*        flag and return. */

    if (iter >= maxit) {
        *info = m;
        return;
    }

/*        this section of the program inspects for */
/*        negligible elements in the s and e arrays.  on */
/*        completion the variables kase and l are set as follows. */

/*           kase = 1     if s(m) and e(l-1) are negligible and l.lt.m */
/*           kase = 2     if s(l) is negligible and l.lt.m */
/*           kase = 3     if e(l-1) is negligible, l.lt.m, and */
/*                        s(l), ..., s(m) are not negligible (qr step). */
/*           kase = 4     if e(m-1) is negligible (convergence). */

    for (l = m-1; l >= 0; --l) {
/*        ...exit */
        if (l == 0) {
            goto L400;
        }
        test = abs(s[l]) + abs(s[l + 1]);
        ztest = test + abs(e[l]);
        if (fsm_ieee_doubles_equal(&ztest, &test)) {
            e[l] = 0.;
/*        ......exit */
            goto L400;
        }
    }
L400:
    if (l != m - 1) {
        goto L410;
    }
    kase = 4;
    goto L480;
L410:
    lp1 = l + 1;
    for (ls = m; ls >= l; --ls) {
/*           ...exit */
        if (ls == l) {
            goto L440;
        }
        test = 0.;
        if (ls != m) {
            test += abs(e[ls]);
        }
        if (ls != l + 1) {
            test += abs(e[ls - 1]);
        }
        ztest = test + abs(s[ls]);
        if (fsm_ieee_doubles_equal(&ztest, &test)) {
            s[ls] = 0.;
/*           ......exit */
            goto L440;
        }
    }
L440:
    if (ls != l) {
        goto L450;
    }
    kase = 3;
    goto L470;
L450:
    if (ls != m) {
        goto L460;
    }
    kase = 1;
    goto L470;
L460:
    kase = 2;
    l = ls;
L470:
L480:
    ++l;

/*        perform the task indicated by kase. */

    switch ((int)kase) {
        case 1:  goto L490;
        case 2:  goto L520;
        case 3:  goto L540;
        case 4:  goto L570;
    }

/*        deflate negligible s(m). */

L490:
    f = e[m - 1];
    e[m - 1] = 0.;
    for (k = m-1; k >= l; --k) {
        t1 = s[k];
        drotg_(&t1, &f, &cs, &sn);
        s[k] = t1;
        if (k == l) {
            goto L500;
        }
        f = -sn * e[k - 1];
        e[k - 1] = cs * e[k - 1];
L500:
        if (wantv) {
            drot_(p, &v[k * v_dim1 + 1], &c__1, &v[m * v_dim1 + 1], &c__1, &cs, &sn);
        }
    }
    goto L360;

/*        split at negligible s(l). */

L520:
    f = e[l - 1];
    e[l - 1] = 0.;
    for (k = l; k <= m; ++k) {
        t1 = s[k];
        drotg_(&t1, &f, &cs, &sn);
        s[k] = t1;
        f = -sn * e[k];
        e[k] = cs * e[k];
        if (wantu) {
            drot_(n, &u[k * u_dim1 + 1], &c__1, &u[(l - 1) * u_dim1 + 1], &c__1, &cs, &sn);
        }
    }
    goto L360;

/*        perform one qr step. */

L540:

/*           calculate the shift. */

    scale = max(max(max(max(abs(s[m]),abs(s[m-1])),abs(e[m-1])),abs(s[l])),abs(e[l]));
    sm = s[m] / scale;
    smm1 = s[m - 1] / scale;
    emm1 = e[m - 1] / scale;
    sl = s[l] / scale;
    el = e[l] / scale;
    b = ((smm1 + sm) * (smm1 - sm) + emm1 * emm1) / 2.;
    d__1 = sm * emm1;
    c = d__1 * d__1;
    shift = 0.;
    if (b == 0. && c == 0.) {
        goto L550;
    }
    shift = sqrt(b * b + c);
    if (b < 0.) {
        shift = -shift;
    }
    shift = c / (b + shift);
L550:
    f = (sl + sm) * (sl - sm) + shift;
    g = sl * el;

/*           chase zeros. */

    for (k = l; k <= m-1; ++k) {
        drotg_(&f, &g, &cs, &sn);
        if (k != l) {
            e[k - 1] = f;
        }
        f = cs * s[k] + sn * e[k];
        e[k] = cs * e[k] - sn * s[k];
        g = sn * s[k + 1];
        s[k + 1] = cs * s[k + 1];
        if (wantv) {
            drot_(p, &v[k * v_dim1 + 1], &c__1, &v[(k + 1) * v_dim1 + 1], &c__1, &cs, &sn);
        }
        drotg_(&f, &g, &cs, &sn);
        s[k] = f;
        f = cs * e[k] + sn * s[k + 1];
        s[k + 1] = -sn * e[k] + cs * s[k + 1];
        g = sn * e[k + 1];
        e[k + 1] = cs * e[k + 1];
        if (wantu && k < *n) {
            drot_(n, &u[k * u_dim1 + 1], &c__1, &u[(k + 1) * u_dim1 + 1], &c__1, &cs, &sn);
        }
    }
    e[m - 1] = f;
    ++iter;
    goto L360;

/*        convergence. */

L570:

/*           make the singular value  positive. */

    if (s[l] >= 0.) {
        goto L580;
    }
    s[l] = -s[l];
    if (wantv) {
        dscal_(p, &c_b44, &v[l * v_dim1 + 1], &c__1);
    }
L580:

/*           order the singular value. */

L590:
    if (l == mm) {
        goto L600;
    }
/*           ...exit */
    if (s[l] >= s[l + 1]) {
        goto L600;
    }
    t = s[l];
    s[l] = s[l + 1];
    s[l + 1] = t;
    if (wantv && l < *p) {
        dswap_(p, &v[l * v_dim1 + 1], &c__1, &v[(l + 1) * v_dim1 + 1], &c__1);
    }
    if (wantu && l < *n) {
        dswap_(n, &u[l * u_dim1 + 1], &c__1, &u[(l + 1) * u_dim1 + 1], &c__1);
    }
    ++l;
    goto L590;
L600:
    iter = 0;
    --m;
    goto L360;
} /* dsvdc_ */

int fsm_ieee_doubles_equal(double *x, double *y)
{
  return *x == *y;
}
