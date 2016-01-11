/* linpack/ssvdc.f -- translated by f2c (version 20050501).
   You must link the resulting object file with libf2c:
        on Microsoft Windows system, link with libf2c.lib;
        on Linux or Unix systems, link with .../path/to/libf2c.a -lm
        or, if you install libf2c.a in a standard place, with -lf2c -lm
        -- in that order, at the end of the command line, as in
                cc *.o -lf2c -lm
        Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

                http://www.netlib.org/f2c/libf2c.zip
*/

/* This code expects correct IEEE rounding behaviour which is not
   always provided.  The source should be built with -ffloat-store.
   A note from the GCC man page:

   -ffloat-store
    Do  not  store floating point variables in registers.  This pre-
    vents undesirable excess precision on machines such as the 68000
    where  the floating registers (of the 68881) keep more precision
    than a double is supposed to have.

    For most programs, the excess precision does only  good,  but  a
    few  programs  rely  on  the precise definition of IEEE floating
    point.  Use `-ffloat-store' for such programs.  */

#ifdef __cplusplus
extern "C" {
#endif
#include "v3p_netlib.h"

/* Table of constant values */

static integer c__1 = 1;
static real c_b44 = (float)-1.;

/*<       subroutine ssvdc(x,ldx,n,p,s,e,u,ldu,v,ldv,work,job,info) >*/
/* Subroutine */ int ssvdc_(real *x, integer *ldx, integer *n, integer *p,
        real *s, real *e, real *u, integer *ldu, real *v, integer *ldv, real *
        work, integer *job, integer *info)
{
    /* System generated locals */
    integer x_dim1, x_offset, u_dim1, u_offset, v_dim1, v_offset, i__1, i__2,
            i__3;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7;

    /* Builtin functions */
    double r_sign(real *, real *), sqrt(doublereal);

    /* Local variables */
    real b, c__, f, g;
    integer i__, j, k, l=0, m;
    real t, t1, el;
    integer kk;
    real cs;
    integer ll, mm, ls=0;
    real sl;
    integer lu;
    real sm, sn;
    integer lm1, mm1, lp1, mp1, nct, ncu, lls, nrt;
    real emm1, smm1;
    integer kase, jobu, iter;
    extern doublereal sdot_(integer *, real *, integer *, real *, integer *);
    real test;
    extern /* Subroutine */ int srot_(integer *, real *, integer *, real *,
            integer *, real *, real *);
    integer nctp1;
    extern doublereal snrm2_(integer *, real *, integer *);
    integer nrtp1;
    real scale;
    extern /* Subroutine */ int sscal_(integer *, real *, real *, integer *);
    real shift;
    integer maxit;
    extern /* Subroutine */ int sswap_(integer *, real *, integer *, real *,
            integer *);
    logical wantu, wantv;
    extern /* Subroutine */ int srotg_(real *, real *, real *, real *),
            saxpy_(integer *, real *, real *, integer *, real *, integer *);
    real ztest;

/*<       integer ldx,n,p,ldu,ldv,job,info >*/
/*<       real x(ldx,1),s(1),e(1),u(ldu,1),v(ldv,1),work(1) >*/


/*     ssvdc is a subroutine to reduce a real nxp matrix x by */
/*     orthogonal transformations u and v to diagonal form.  the */
/*     diagonal elements s(i) are the singular values of x.  the */
/*     columns of u are the corresponding left singular vectors, */
/*     and the columns of v the right singular vectors. */

/*     on entry */

/*         x         real(ldx,p), where ldx.ge.n. */
/*                   x contains the matrix whose singular value */
/*                   decomposition is to be computed.  x is */
/*                   destroyed by ssvdc. */

/*         ldx       integer. */
/*                   ldx is the leading dimension of the array x. */

/*         n         integer. */
/*                   n is the number of rows of the matrix x. */

/*         p         integer. */
/*                   p is the number of columns of the matrix x. */

/*         ldu       integer. */
/*                   ldu is the leading dimension of the array u. */
/*                   (see below). */

/*         ldv       integer. */
/*                   ldv is the leading dimension of the array v. */
/*                   (see below). */

/*         work      real(n). */
/*                   work is a scratch array. */

/*         job       integer. */
/*                   job controls the computation of the singular */
/*                   vectors.  it has the decimal expansion ab */
/*                   with the following meaning */

/*                        a.eq.0    do not compute the left singular */
/*                                  vectors. */
/*                        a.eq.1    return the n left singular vectors */
/*                                  in u. */
/*                        a.ge.2    return the first min(n,p) singular */
/*                                  vectors in u. */
/*                        b.eq.0    do not compute the right singular */
/*                                  vectors. */
/*                        b.eq.1    return the right singular vectors */
/*                                  in v. */

/*     on return */

/*         s         real(mm), where mm=min(n+1,p). */
/*                   the first min(n,p) entries of s contain the */
/*                   singular values of x arranged in descending */
/*                   order of magnitude. */

/*         e         real(p). */
/*                   e ordinarily contains zeros.  however see the */
/*                   discussion of info for exceptions. */

/*         u         real(ldu,k), where ldu.ge.n.  if joba.eq.1 then */
/*                                   k.eq.n, if joba.ge.2 then */
/*                                   k.eq.min(n,p). */
/*                   u contains the matrix of left singular vectors. */
/*                   u is not referenced if joba.eq.0.  if n.le.p */
/*                   or if joba.eq.2, then u may be identified with x */
/*                   in the subroutine call. */

/*         v         real(ldv,p), where ldv.ge.p. */
/*                   v contains the matrix of right singular vectors. */
/*                   v is not referenced if job.eq.0.  if p.le.n, */
/*                   then v may be identified with x in the */
/*                   subroutine call. */

/*         info      integer. */
/*                   the singular values (and their corresponding */
/*                   singular vectors) s(info+1),s(info+2),...,s(m) */
/*                   are correct (here m=min(n,p)).  thus if */
/*                   info.eq.0, all the singular values and their */
/*                   vectors are correct.  in any event, the matrix */
/*                   b = trans(u)*x*v is the bidiagonal matrix */
/*                   with the elements of s on its diagonal and the */
/*                   elements of e on its super-diagonal (trans(u) */
/*                   is the transpose of u).  thus the singular */
/*                   values of x and b are the same. */

/*     linpack. this version dated 03/19/79 . */
/*              correction to shift calculation made 2/85. */
/*     g.w. stewart, university of maryland, argonne national lab. */

/*     ***** uses the following functions and subprograms. */

/*     external srot */
/*     blas saxpy,sdot,sscal,sswap,snrm2,srotg */
/*     fortran abs,amax1,max0,min0,mod,sqrt */

/*     internal variables */

/*<    >*/
/*<       real sdot,t,r >*/
/*<    >*/
/*<       logical wantu,wantv >*/


/*     set the maximum number of iterations. */

/*<       maxit = 1000 >*/
    /* Parameter adjustments */
    x_dim1 = *ldx;
    x_offset = 1 + x_dim1;
    x -= x_offset;
    --s;
    --e;
    u_dim1 = *ldu;
    u_offset = 1 + u_dim1;
    u -= u_offset;
    v_dim1 = *ldv;
    v_offset = 1 + v_dim1;
    v -= v_offset;
    --work;

    /* Function Body */
    maxit = 1000;

/*     determine what is to be computed. */

/*<       wantu = .false. >*/
    wantu = FALSE_;
/*<       wantv = .false. >*/
    wantv = FALSE_;
/*<       jobu = mod(job,100)/10 >*/
    jobu = *job % 100 / 10;
/*<       ncu = n >*/
    ncu = *n;
/*<       if (jobu .gt. 1) ncu = min0(n,p) >*/
    if (jobu > 1) {
        ncu = min(*n,*p);
    }
/*<       if (jobu .ne. 0) wantu = .true. >*/
    if (jobu != 0) {
        wantu = TRUE_;
    }
/*<       if (mod(job,10) .ne. 0) wantv = .true. >*/
    if (*job % 10 != 0) {
        wantv = TRUE_;
    }

/*     reduce x to bidiagonal form, storing the diagonal elements */
/*     in s and the super-diagonal elements in e. */

/*<       info = 0 >*/
    *info = 0;
/*<       nct = min0(n-1,p) >*/
/* Computing MIN */
    i__1 = *n - 1;
    nct = min(i__1,*p);
/*<       nrt = max0(0,min0(p-2,n)) >*/
/* Computing MAX */
/* Computing MIN */
    i__3 = *p - 2;
    i__1 = 0, i__2 = min(i__3,*n);
    nrt = max(i__1,i__2);
/*<       lu = max0(nct,nrt) >*/
    lu = max(nct,nrt);
/*<       if (lu .lt. 1) go to 170 >*/
    if (lu < 1) {
        goto L170;
    }
/*<       do 160 l = 1, lu >*/
    i__1 = lu;
    for (l = 1; l <= i__1; ++l) {
/*<          lp1 = l + 1 >*/
        lp1 = l + 1;
/*<          if (l .gt. nct) go to 20 >*/
        if (l > nct) {
            goto L20;
        }

/*           compute the transformation for the l-th column and */
/*           place the l-th diagonal in s(l). */

/*<             s(l) = snrm2(n-l+1,x(l,l),1) >*/
        i__2 = *n - l + 1;
        s[l] = snrm2_(&i__2, &x[l + l * x_dim1], &c__1);
/*<             if (s(l) .eq. 0.0e0) go to 10 >*/
        if (s[l] == (float)0.) {
            goto L10;
        }
/*<                if (x(l,l) .ne. 0.0e0) s(l) = sign(s(l),x(l,l)) >*/
        if (x[l + l * x_dim1] != (float)0.) {
            s[l] = r_sign(&s[l], &x[l + l * x_dim1]);
        }
/*<                call sscal(n-l+1,1.0e0/s(l),x(l,l),1) >*/
        i__2 = *n - l + 1;
        r__1 = (float)1. / s[l];
        sscal_(&i__2, &r__1, &x[l + l * x_dim1], &c__1);
/*<                x(l,l) = 1.0e0 + x(l,l) >*/
        x[l + l * x_dim1] += (float)1.;
/*<    10       continue >*/
L10:
/*<             s(l) = -s(l) >*/
        s[l] = -s[l];
/*<    20    continue >*/
L20:
/*<          if (p .lt. lp1) go to 50 >*/
        if (*p < lp1) {
            goto L50;
        }
/*<          do 40 j = lp1, p >*/
        i__2 = *p;
        for (j = lp1; j <= i__2; ++j) {
/*<             if (l .gt. nct) go to 30 >*/
            if (l > nct) {
                goto L30;
            }
/*<             if (s(l) .eq. 0.0e0) go to 30 >*/
            if (s[l] == (float)0.) {
                goto L30;
            }

/*              apply the transformation. */

/*<                t = -sdot(n-l+1,x(l,l),1,x(l,j),1)/x(l,l) >*/
            i__3 = *n - l + 1;
            t = -sdot_(&i__3, &x[l + l * x_dim1], &c__1, &x[l + j * x_dim1], &
                    c__1) / x[l + l * x_dim1];
/*<                call saxpy(n-l+1,t,x(l,l),1,x(l,j),1) >*/
            i__3 = *n - l + 1;
            saxpy_(&i__3, &t, &x[l + l * x_dim1], &c__1, &x[l + j * x_dim1], &
                    c__1);
/*<    30       continue >*/
L30:

/*           place the l-th row of x into  e for the */
/*           subsequent calculation of the row transformation. */

/*<             e(j) = x(l,j) >*/
            e[j] = x[l + j * x_dim1];
/*<    40    continue >*/
/* L40: */
        }
/*<    50    continue >*/
L50:
/*<          if (.not.wantu .or. l .gt. nct) go to 70 >*/
        if (! wantu || l > nct) {
            goto L70;
        }

/*           place the transformation in u for subsequent back */
/*           multiplication. */

/*<             do 60 i = l, n >*/
        i__2 = *n;
        for (i__ = l; i__ <= i__2; ++i__) {
/*<                u(i,l) = x(i,l) >*/
            u[i__ + l * u_dim1] = x[i__ + l * x_dim1];
/*<    60       continue >*/
/* L60: */
        }
/*<    70    continue >*/
L70:
/*<          if (l .gt. nrt) go to 150 >*/
        if (l > nrt) {
            goto L150;
        }

/*           compute the l-th row transformation and place the */
/*           l-th super-diagonal in e(l). */

/*<             e(l) = snrm2(p-l,e(lp1),1) >*/
        i__2 = *p - l;
        e[l] = snrm2_(&i__2, &e[lp1], &c__1);
/*<             if (e(l) .eq. 0.0e0) go to 80 >*/
        if (e[l] == (float)0.) {
            goto L80;
        }
/*<                if (e(lp1) .ne. 0.0e0) e(l) = sign(e(l),e(lp1)) >*/
        if (e[lp1] != (float)0.) {
            e[l] = r_sign(&e[l], &e[lp1]);
        }
/*<                call sscal(p-l,1.0e0/e(l),e(lp1),1) >*/
        i__2 = *p - l;
        r__1 = (float)1. / e[l];
        sscal_(&i__2, &r__1, &e[lp1], &c__1);
/*<                e(lp1) = 1.0e0 + e(lp1) >*/
        e[lp1] += (float)1.;
/*<    80       continue >*/
L80:
/*<             e(l) = -e(l) >*/
        e[l] = -e[l];
/*<             if (lp1 .gt. n .or. e(l) .eq. 0.0e0) go to 120 >*/
        if (lp1 > *n || e[l] == (float)0.) {
            goto L120;
        }

/*              apply the transformation. */

/*<                do 90 i = lp1, n >*/
        i__2 = *n;
        for (i__ = lp1; i__ <= i__2; ++i__) {
/*<                   work(i) = 0.0e0 >*/
            work[i__] = (float)0.;
/*<    90          continue >*/
/* L90: */
        }
/*<                do 100 j = lp1, p >*/
        i__2 = *p;
        for (j = lp1; j <= i__2; ++j) {
/*<                   call saxpy(n-l,e(j),x(lp1,j),1,work(lp1),1) >*/
            i__3 = *n - l;
            saxpy_(&i__3, &e[j], &x[lp1 + j * x_dim1], &c__1, &work[lp1], &
                    c__1);
/*<   100          continue >*/
/* L100: */
        }
/*<                do 110 j = lp1, p >*/
        i__2 = *p;
        for (j = lp1; j <= i__2; ++j) {
/*<                   call saxpy(n-l,-e(j)/e(lp1),work(lp1),1,x(lp1,j),1) >*/
            i__3 = *n - l;
            r__1 = -e[j] / e[lp1];
            saxpy_(&i__3, &r__1, &work[lp1], &c__1, &x[lp1 + j * x_dim1], &
                    c__1);
/*<   110          continue >*/
/* L110: */
        }
/*<   120       continue >*/
L120:
/*<             if (.not.wantv) go to 140 >*/
        if (! wantv) {
            goto L140;
        }

/*              place the transformation in v for subsequent */
/*              back multiplication. */

/*<                do 130 i = lp1, p >*/
        i__2 = *p;
        for (i__ = lp1; i__ <= i__2; ++i__) {
/*<                   v(i,l) = e(i) >*/
            v[i__ + l * v_dim1] = e[i__];
/*<   130          continue >*/
/* L130: */
        }
/*<   140       continue >*/
L140:
/*<   150    continue >*/
L150:
/*<   160 continue >*/
/* L160: */
        ;
    }
/*<   170 continue >*/
L170:

/*     set up the final bidiagonal matrix or order m. */

/*<       m = min0(p,n+1) >*/
/* Computing MIN */
    i__1 = *p, i__2 = *n + 1;
    m = min(i__1,i__2);
/*<       nctp1 = nct + 1 >*/
    nctp1 = nct + 1;
/*<       nrtp1 = nrt + 1 >*/
    nrtp1 = nrt + 1;
/*<       if (nct .lt. p) s(nctp1) = x(nctp1,nctp1) >*/
    if (nct < *p) {
        s[nctp1] = x[nctp1 + nctp1 * x_dim1];
    }
/*<       if (n .lt. m) s(m) = 0.0e0 >*/
    if (*n < m) {
        s[m] = (float)0.;
    }
/*<       if (nrtp1 .lt. m) e(nrtp1) = x(nrtp1,m) >*/
    if (nrtp1 < m) {
        e[nrtp1] = x[nrtp1 + m * x_dim1];
    }
/*<       e(m) = 0.0e0 >*/
    e[m] = (float)0.;

/*     if required, generate u. */

/*<       if (.not.wantu) go to 300 >*/
    if (! wantu) {
        goto L300;
    }
/*<          if (ncu .lt. nctp1) go to 200 >*/
    if (ncu < nctp1) {
        goto L200;
    }
/*<          do 190 j = nctp1, ncu >*/
    i__1 = ncu;
    for (j = nctp1; j <= i__1; ++j) {
/*<             do 180 i = 1, n >*/
        i__2 = *n;
        for (i__ = 1; i__ <= i__2; ++i__) {
/*<                u(i,j) = 0.0e0 >*/
            u[i__ + j * u_dim1] = (float)0.;
/*<   180       continue >*/
/* L180: */
        }
/*<             u(j,j) = 1.0e0 >*/
        u[j + j * u_dim1] = (float)1.;
/*<   190    continue >*/
/* L190: */
    }
/*<   200    continue >*/
L200:
/*<          if (nct .lt. 1) go to 290 >*/
    if (nct < 1) {
        goto L290;
    }
/*<          do 280 ll = 1, nct >*/
    i__1 = nct;
    for (ll = 1; ll <= i__1; ++ll) {
/*<             l = nct - ll + 1 >*/
        l = nct - ll + 1;
/*<             if (s(l) .eq. 0.0e0) go to 250 >*/
        if (s[l] == (float)0.) {
            goto L250;
        }
/*<                lp1 = l + 1 >*/
        lp1 = l + 1;
/*<                if (ncu .lt. lp1) go to 220 >*/
        if (ncu < lp1) {
            goto L220;
        }
/*<                do 210 j = lp1, ncu >*/
        i__2 = ncu;
        for (j = lp1; j <= i__2; ++j) {
/*<                   t = -sdot(n-l+1,u(l,l),1,u(l,j),1)/u(l,l) >*/
            i__3 = *n - l + 1;
            t = -sdot_(&i__3, &u[l + l * u_dim1], &c__1, &u[l + j * u_dim1], &
                    c__1) / u[l + l * u_dim1];
/*<                   call saxpy(n-l+1,t,u(l,l),1,u(l,j),1) >*/
            i__3 = *n - l + 1;
            saxpy_(&i__3, &t, &u[l + l * u_dim1], &c__1, &u[l + j * u_dim1], &
                    c__1);
/*<   210          continue >*/
/* L210: */
        }
/*<   220          continue >*/
L220:
/*<                call sscal(n-l+1,-1.0e0,u(l,l),1) >*/
        i__2 = *n - l + 1;
        sscal_(&i__2, &c_b44, &u[l + l * u_dim1], &c__1);
/*<                u(l,l) = 1.0e0 + u(l,l) >*/
        u[l + l * u_dim1] += (float)1.;
/*<                lm1 = l - 1 >*/
        lm1 = l - 1;
/*<                if (lm1 .lt. 1) go to 240 >*/
        if (lm1 < 1) {
            goto L240;
        }
/*<                do 230 i = 1, lm1 >*/
        i__2 = lm1;
        for (i__ = 1; i__ <= i__2; ++i__) {
/*<                   u(i,l) = 0.0e0 >*/
            u[i__ + l * u_dim1] = (float)0.;
/*<   230          continue >*/
/* L230: */
        }
/*<   240          continue >*/
L240:
/*<             go to 270 >*/
        goto L270;
/*<   250       continue >*/
L250:
/*<                do 260 i = 1, n >*/
        i__2 = *n;
        for (i__ = 1; i__ <= i__2; ++i__) {
/*<                   u(i,l) = 0.0e0 >*/
            u[i__ + l * u_dim1] = (float)0.;
/*<   260          continue >*/
/* L260: */
        }
/*<                u(l,l) = 1.0e0 >*/
        u[l + l * u_dim1] = (float)1.;
/*<   270       continue >*/
L270:
/*<   280    continue >*/
/* L280: */
        ;
    }
/*<   290    continue >*/
L290:
/*<   300 continue >*/
L300:

/*     if it is required, generate v. */

/*<       if (.not.wantv) go to 350 >*/
    if (! wantv) {
        goto L350;
    }
/*<          do 340 ll = 1, p >*/
    i__1 = *p;
    for (ll = 1; ll <= i__1; ++ll) {
/*<             l = p - ll + 1 >*/
        l = *p - ll + 1;
/*<             lp1 = l + 1 >*/
        lp1 = l + 1;
/*<             if (l .gt. nrt) go to 320 >*/
        if (l > nrt) {
            goto L320;
        }
/*<             if (e(l) .eq. 0.0e0) go to 320 >*/
        if (e[l] == (float)0.) {
            goto L320;
        }
/*<                do 310 j = lp1, p >*/
        i__2 = *p;
        for (j = lp1; j <= i__2; ++j) {
/*<                   t = -sdot(p-l,v(lp1,l),1,v(lp1,j),1)/v(lp1,l) >*/
            i__3 = *p - l;
            t = -sdot_(&i__3, &v[lp1 + l * v_dim1], &c__1, &v[lp1 + j *
                    v_dim1], &c__1) / v[lp1 + l * v_dim1];
/*<                   call saxpy(p-l,t,v(lp1,l),1,v(lp1,j),1) >*/
            i__3 = *p - l;
            saxpy_(&i__3, &t, &v[lp1 + l * v_dim1], &c__1, &v[lp1 + j *
                    v_dim1], &c__1);
/*<   310          continue >*/
/* L310: */
        }
/*<   320       continue >*/
L320:
/*<             do 330 i = 1, p >*/
        i__2 = *p;
        for (i__ = 1; i__ <= i__2; ++i__) {
/*<                v(i,l) = 0.0e0 >*/
            v[i__ + l * v_dim1] = (float)0.;
/*<   330       continue >*/
/* L330: */
        }
/*<             v(l,l) = 1.0e0 >*/
        v[l + l * v_dim1] = (float)1.;
/*<   340    continue >*/
/* L340: */
    }
/*<   350 continue >*/
L350:

/*     main iteration loop for the singular values. */

/*<       mm = m >*/
    mm = m;
/*<       iter = 0 >*/
    iter = 0;
/*<   360 continue >*/
L360:

/*        quit if all the singular values have been found. */

/*     ...exit */
/*<          if (m .eq. 0) go to 620 >*/
    if (m == 0) {
        goto L620;
    }

/*        if too many iterations have been performed, set */
/*        flag and return. */

/*<          if (iter .lt. maxit) go to 370 >*/
    if (iter < maxit) {
        goto L370;
    }
/*<             info = m >*/
    *info = m;
/*     ......exit */
/*<             go to 620 >*/
    goto L620;
/*<   370    continue >*/
L370:

/*        this section of the program inspects for */
/*        negligible elements in the s and e arrays.  on */
/*        completion the variables kase and l are set as follows. */

/*           kase = 1     if s(m) and e(l-1) are negligible and l.lt.m */
/*           kase = 2     if s(l) is negligible and l.lt.m */
/*           kase = 3     if e(l-1) is negligible, l.lt.m, and */
/*                        s(l), ..., s(m) are not negligible (qr step). */
/*           kase = 4     if e(m-1) is negligible (convergence). */

/*<          do 390 ll = 1, m >*/
    i__1 = m;
    for (ll = 1; ll <= i__1; ++ll) {
/*<             l = m - ll >*/
        l = m - ll;
/*        ...exit */
/*<             if (l .eq. 0) go to 400 >*/
        if (l == 0) {
            goto L400;
        }
/*<             test = abs(s(l)) + abs(s(l+1)) >*/
        test = (r__1 = s[l], dabs(r__1)) + (r__2 = s[l + 1], dabs(r__2));
/*<             ztest = test + abs(e(l)) >*/
        ztest = test + (r__1 = e[l], dabs(r__1));
/*<             if (ztest .ne. test) go to 380 >*/
        if (ztest != test) {
            goto L380;
        }
/*<                e(l) = 0.0e0 >*/
        e[l] = (float)0.;
/*        ......exit */
/*<                go to 400 >*/
        goto L400;
/*<   380       continue >*/
L380:
/*<   390    continue >*/
/* L390: */
        ;
    }
/*<   400    continue >*/
L400:
/*<          if (l .ne. m - 1) go to 410 >*/
    if (l != m - 1) {
        goto L410;
    }
/*<             kase = 4 >*/
    kase = 4;
/*<          go to 480 >*/
    goto L480;
/*<   410    continue >*/
L410:
/*<             lp1 = l + 1 >*/
    lp1 = l + 1;
/*<             mp1 = m + 1 >*/
    mp1 = m + 1;
/*<             do 430 lls = lp1, mp1 >*/
    i__1 = mp1;
    for (lls = lp1; lls <= i__1; ++lls) {
/*<                ls = m - lls + lp1 >*/
        ls = m - lls + lp1;
/*           ...exit */
/*<                if (ls .eq. l) go to 440 >*/
        if (ls == l) {
            goto L440;
        }
/*<                test = 0.0e0 >*/
        test = (float)0.;
/*<                if (ls .ne. m) test = test + abs(e(ls)) >*/
        if (ls != m) {
            test += (r__1 = e[ls], dabs(r__1));
        }
/*<                if (ls .ne. l + 1) test = test + abs(e(ls-1)) >*/
        if (ls != l + 1) {
            test += (r__1 = e[ls - 1], dabs(r__1));
        }
/*<                ztest = test + abs(s(ls)) >*/
        ztest = test + (r__1 = s[ls], dabs(r__1));
/*<                if (ztest .ne. test) go to 420 >*/
        if (ztest != test) {
            goto L420;
        }
/*<                   s(ls) = 0.0e0 >*/
        s[ls] = (float)0.;
/*           ......exit */
/*<                   go to 440 >*/
        goto L440;
/*<   420          continue >*/
L420:
/*<   430       continue >*/
/* L430: */
        ;
    }
/*<   440       continue >*/
L440:
/*<             if (ls .ne. l) go to 450 >*/
    if (ls != l) {
        goto L450;
    }
/*<                kase = 3 >*/
    kase = 3;
/*<             go to 470 >*/
    goto L470;
/*<   450       continue >*/
L450:
/*<             if (ls .ne. m) go to 460 >*/
    if (ls != m) {
        goto L460;
    }
/*<                kase = 1 >*/
    kase = 1;
/*<             go to 470 >*/
    goto L470;
/*<   460       continue >*/
L460:
/*<                kase = 2 >*/
    kase = 2;
/*<                l = ls >*/
    l = ls;
/*<   470       continue >*/
L470:
/*<   480    continue >*/
L480:
/*<          l = l + 1 >*/
    ++l;

/*        perform the task indicated by kase. */

/*<          go to (490,520,540,570), kase >*/
    switch (kase) {
        case 1:  goto L490;
        case 2:  goto L520;
        case 3:  goto L540;
        case 4:  goto L570;
    }

/*        deflate negligible s(m). */

/*<   490    continue >*/
L490:
/*<             mm1 = m - 1 >*/
    mm1 = m - 1;
/*<             f = e(m-1) >*/
    f = e[m - 1];
/*<             e(m-1) = 0.0e0 >*/
    e[m - 1] = (float)0.;
/*<             do 510 kk = l, mm1 >*/
    i__1 = mm1;
    for (kk = l; kk <= i__1; ++kk) {
/*<                k = mm1 - kk + l >*/
        k = mm1 - kk + l;
/*<                t1 = s(k) >*/
        t1 = s[k];
/*<                call srotg(t1,f,cs,sn) >*/
        srotg_(&t1, &f, &cs, &sn);
/*<                s(k) = t1 >*/
        s[k] = t1;
/*<                if (k .eq. l) go to 500 >*/
        if (k == l) {
            goto L500;
        }
/*<                   f = -sn*e(k-1) >*/
        f = -sn * e[k - 1];
/*<                   e(k-1) = cs*e(k-1) >*/
        e[k - 1] = cs * e[k - 1];
/*<   500          continue >*/
L500:
/*<                if (wantv) call srot(p,v(1,k),1,v(1,m),1,cs,sn) >*/
        if (wantv) {
            srot_(p, &v[k * v_dim1 + 1], &c__1, &v[m * v_dim1 + 1], &c__1, &
                    cs, &sn);
        }
/*<   510       continue >*/
/* L510: */
    }
/*<          go to 610 >*/
    goto L610;

/*        split at negligible s(l). */

/*<   520    continue >*/
L520:
/*<             f = e(l-1) >*/
    f = e[l - 1];
/*<             e(l-1) = 0.0e0 >*/
    e[l - 1] = (float)0.;
/*<             do 530 k = l, m >*/
    i__1 = m;
    for (k = l; k <= i__1; ++k) {
/*<                t1 = s(k) >*/
        t1 = s[k];
/*<                call srotg(t1,f,cs,sn) >*/
        srotg_(&t1, &f, &cs, &sn);
/*<                s(k) = t1 >*/
        s[k] = t1;
/*<                f = -sn*e(k) >*/
        f = -sn * e[k];
/*<                e(k) = cs*e(k) >*/
        e[k] = cs * e[k];
/*<                if (wantu) call srot(n,u(1,k),1,u(1,l-1),1,cs,sn) >*/
        if (wantu) {
            srot_(n, &u[k * u_dim1 + 1], &c__1, &u[(l - 1) * u_dim1 + 1], &
                    c__1, &cs, &sn);
        }
/*<   530       continue >*/
/* L530: */
    }
/*<          go to 610 >*/
    goto L610;

/*        perform one qr step. */

/*<   540    continue >*/
L540:

/*           calculate the shift. */

/*<    >*/
/* Computing MAX */
    r__6 = (r__1 = s[m], dabs(r__1)), r__7 = (r__2 = s[m - 1], dabs(r__2)),
            r__6 = max(r__6,r__7), r__7 = (r__3 = e[m - 1], dabs(r__3)), r__6
            = max(r__6,r__7), r__7 = (r__4 = s[l], dabs(r__4)), r__6 = max(
            r__6,r__7), r__7 = (r__5 = e[l], dabs(r__5));
    scale = dmax(r__6,r__7);
/*<             sm = s(m)/scale >*/
    sm = s[m] / scale;
/*<             smm1 = s(m-1)/scale >*/
    smm1 = s[m - 1] / scale;
/*<             emm1 = e(m-1)/scale >*/
    emm1 = e[m - 1] / scale;
/*<             sl = s(l)/scale >*/
    sl = s[l] / scale;
/*<             el = e(l)/scale >*/
    el = e[l] / scale;
/*<             b = ((smm1 + sm)*(smm1 - sm) + emm1**2)/2.0e0 >*/
/* Computing 2nd power */
    r__1 = emm1;
    b = ((smm1 + sm) * (smm1 - sm) + r__1 * r__1) / (float)2.;
/*<             c = (sm*emm1)**2 >*/
/* Computing 2nd power */
    r__1 = sm * emm1;
    c__ = r__1 * r__1;
/*<             shift = 0.0e0 >*/
    shift = (float)0.;
/*<             if (b .eq. 0.0e0 .and. c .eq. 0.0e0) go to 550 >*/
    if (b == (float)0. && c__ == (float)0.) {
        goto L550;
    }
/*<                shift = sqrt(b**2+c) >*/
/* Computing 2nd power */
    r__1 = b;
    shift = sqrt(r__1 * r__1 + c__);
/*<                if (b .lt. 0.0e0) shift = -shift >*/
    if (b < (float)0.) {
        shift = -shift;
    }
/*<                shift = c/(b + shift) >*/
    shift = c__ / (b + shift);
/*<   550       continue >*/
L550:
/*<             f = (sl + sm)*(sl - sm) + shift >*/
    f = (sl + sm) * (sl - sm) + shift;
/*<             g = sl*el >*/
    g = sl * el;

/*           chase zeros. */

/*<             mm1 = m - 1 >*/
    mm1 = m - 1;
/*<             do 560 k = l, mm1 >*/
    i__1 = mm1;
    for (k = l; k <= i__1; ++k) {
/*<                call srotg(f,g,cs,sn) >*/
        srotg_(&f, &g, &cs, &sn);
/*<                if (k .ne. l) e(k-1) = f >*/
        if (k != l) {
            e[k - 1] = f;
        }
/*<                f = cs*s(k) + sn*e(k) >*/
        f = cs * s[k] + sn * e[k];
/*<                e(k) = cs*e(k) - sn*s(k) >*/
        e[k] = cs * e[k] - sn * s[k];
/*<                g = sn*s(k+1) >*/
        g = sn * s[k + 1];
/*<                s(k+1) = cs*s(k+1) >*/
        s[k + 1] = cs * s[k + 1];
/*<                if (wantv) call srot(p,v(1,k),1,v(1,k+1),1,cs,sn) >*/
        if (wantv) {
            srot_(p, &v[k * v_dim1 + 1], &c__1, &v[(k + 1) * v_dim1 + 1], &
                    c__1, &cs, &sn);
        }
/*<                call srotg(f,g,cs,sn) >*/
        srotg_(&f, &g, &cs, &sn);
/*<                s(k) = f >*/
        s[k] = f;
/*<                f = cs*e(k) + sn*s(k+1) >*/
        f = cs * e[k] + sn * s[k + 1];
/*<                s(k+1) = -sn*e(k) + cs*s(k+1) >*/
        s[k + 1] = -sn * e[k] + cs * s[k + 1];
/*<                g = sn*e(k+1) >*/
        g = sn * e[k + 1];
/*<                e(k+1) = cs*e(k+1) >*/
        e[k + 1] = cs * e[k + 1];
/*<    >*/
        if (wantu && k < *n) {
            srot_(n, &u[k * u_dim1 + 1], &c__1, &u[(k + 1) * u_dim1 + 1], &
                    c__1, &cs, &sn);
        }
/*<   560       continue >*/
/* L560: */
    }
/*<             e(m-1) = f >*/
    e[m - 1] = f;
/*<             iter = iter + 1 >*/
    ++iter;
/*<          go to 610 >*/
    goto L610;

/*        convergence. */

/*<   570    continue >*/
L570:

/*           make the singular value  positive. */

/*<             if (s(l) .ge. 0.0e0) go to 580 >*/
    if (s[l] >= (float)0.) {
        goto L580;
    }
/*<                s(l) = -s(l) >*/
    s[l] = -s[l];
/*<                if (wantv) call sscal(p,-1.0e0,v(1,l),1) >*/
    if (wantv) {
        sscal_(p, &c_b44, &v[l * v_dim1 + 1], &c__1);
    }
/*<   580       continue >*/
L580:

/*           order the singular value. */

/*<   590       if (l .eq. mm) go to 600 >*/
L590:
    if (l == mm) {
        goto L600;
    }
/*           ...exit */
/*<                if (s(l) .ge. s(l+1)) go to 600 >*/
    if (s[l] >= s[l + 1]) {
        goto L600;
    }
/*<                t = s(l) >*/
    t = s[l];
/*<                s(l) = s(l+1) >*/
    s[l] = s[l + 1];
/*<                s(l+1) = t >*/
    s[l + 1] = t;
/*<    >*/
    if (wantv && l < *p) {
        sswap_(p, &v[l * v_dim1 + 1], &c__1, &v[(l + 1) * v_dim1 + 1], &c__1);
    }
/*<    >*/
    if (wantu && l < *n) {
        sswap_(n, &u[l * u_dim1 + 1], &c__1, &u[(l + 1) * u_dim1 + 1], &c__1);
    }
/*<                l = l + 1 >*/
    ++l;
/*<             go to 590 >*/
    goto L590;
/*<   600       continue >*/
L600:
/*<             iter = 0 >*/
    iter = 0;
/*<             m = m - 1 >*/
    --m;
/*<   610    continue >*/
L610:
/*<       go to 360 >*/
    goto L360;
/*<   620 continue >*/
L620:
/*<       return >*/
    return 0;
/*<       end >*/
} /* ssvdc_ */

#ifdef __cplusplus
        }
#endif
