/* lapack/double/dlatdf.f -- translated by f2c (version 20050501).
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

/* Table of constant values */

static integer c__1 = 1;
static integer c_n1 = -1;
static doublereal c_b23 = 1.;
static doublereal c_b37 = -1.;

/*<    >*/
/* Subroutine */ int dlatdf_(integer *ijob, integer *n, doublereal *z__,
        integer *ldz, doublereal *rhs, doublereal *rdsum, doublereal *rdscal,
        integer *ipiv, integer *jpiv)
{
    /* System generated locals */
    integer z_dim1, z_offset, i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    double sqrt(doublereal);

    /* Local variables */
    integer i__, j, k;
    doublereal bm, bp, xm[8], xp[8];
    extern doublereal ddot_(integer *, doublereal *, integer *, doublereal *,
            integer *);
    integer info;
    doublereal temp, work[32];
    extern /* Subroutine */ int dscal_(integer *, doublereal *, doublereal *,
            integer *);
    extern doublereal dasum_(integer *, doublereal *, integer *);
    doublereal pmone;
    extern /* Subroutine */ int dcopy_(integer *, doublereal *, integer *,
            doublereal *, integer *), daxpy_(integer *, doublereal *,
            doublereal *, integer *, doublereal *, integer *);
    doublereal sminu;
    integer iwork[8];
    doublereal splus;
    extern /* Subroutine */ int dgesc2_(integer *, doublereal *, integer *,
            doublereal *, integer *, integer *, doublereal *), dgecon_(char *,
             integer *, doublereal *, integer *, doublereal *, doublereal *,
            doublereal *, integer *, integer *, ftnlen), dlassq_(integer *,
            doublereal *, integer *, doublereal *, doublereal *), dlaswp_(
            integer *, doublereal *, integer *, integer *, integer *, integer
            *, integer *);


/*  -- LAPACK auxiliary routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     June 30, 1999 */

/*     .. Scalar Arguments .. */
/*<       INTEGER            IJOB, LDZ, N >*/
/*<       DOUBLE PRECISION   RDSCAL, RDSUM >*/
/*     .. */
/*     .. Array Arguments .. */
/*<       INTEGER            IPIV( * ), JPIV( * ) >*/
/*<       DOUBLE PRECISION   RHS( * ), Z( LDZ, * ) >*/
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLATDF uses the LU factorization of the n-by-n matrix Z computed by */
/*  DGETC2 and computes a contribution to the reciprocal Dif-estimate */
/*  by solving Z * x = b for x, and choosing the r.h.s. b such that */
/*  the norm of x is as large as possible. On entry RHS = b holds the */
/*  contribution from earlier solved sub-systems, and on return RHS = x. */

/*  The factorization of Z returned by DGETC2 has the form Z = P*L*U*Q, */
/*  where P and Q are permutation matrices. L is lower triangular with */
/*  unit diagonal elements and U is upper triangular. */

/*  Arguments */
/*  ========= */

/*  IJOB    (input) INTEGER */
/*          IJOB = 2: First compute an approximative null-vector e */
/*              of Z using DGECON, e is normalized and solve for */
/*              Zx = +-e - f with the sign giving the greater value */
/*              of 2-norm(x). About 5 times as expensive as Default. */
/*          IJOB .ne. 2: Local look ahead strategy where all entries of */
/*              the r.h.s. b is choosen as either +1 or -1 (Default). */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix Z. */

/*  Z       (input) DOUBLE PRECISION array, dimension (LDZ, N) */
/*          On entry, the LU part of the factorization of the n-by-n */
/*          matrix Z computed by DGETC2:  Z = P * L * U * Q */

/*  LDZ     (input) INTEGER */
/*          The leading dimension of the array Z.  LDA >= max(1, N). */

/*  RHS     (input/output) DOUBLE PRECISION array, dimension N. */
/*          On entry, RHS contains contributions from other subsystems. */
/*          On exit, RHS contains the solution of the subsystem with */
/*          entries acoording to the value of IJOB (see above). */

/*  RDSUM   (input/output) DOUBLE PRECISION */
/*          On entry, the sum of squares of computed contributions to */
/*          the Dif-estimate under computation by DTGSYL, where the */
/*          scaling factor RDSCAL (see below) has been factored out. */
/*          On exit, the corresponding sum of squares updated with the */
/*          contributions from the current sub-system. */
/*          If TRANS = 'T' RDSUM is not touched. */
/*          NOTE: RDSUM only makes sense when DTGSY2 is called by STGSYL. */

/*  RDSCAL  (input/output) DOUBLE PRECISION */
/*          On entry, scaling factor used to prevent overflow in RDSUM. */
/*          On exit, RDSCAL is updated w.r.t. the current contributions */
/*          in RDSUM. */
/*          If TRANS = 'T', RDSCAL is not touched. */
/*          NOTE: RDSCAL only makes sense when DTGSY2 is called by */
/*                DTGSYL. */

/*  IPIV    (input) INTEGER array, dimension (N). */
/*          The pivot indices; for 1 <= i <= N, row i of the */
/*          matrix has been interchanged with row IPIV(i). */

/*  JPIV    (input) INTEGER array, dimension (N). */
/*          The pivot indices; for 1 <= j <= N, column j of the */
/*          matrix has been interchanged with column JPIV(j). */

/*  Further Details */
/*  =============== */

/*  Based on contributions by */
/*     Bo Kagstrom and Peter Poromaa, Department of Computing Science, */
/*     Umea University, S-901 87 Umea, Sweden. */

/*  This routine is a further developed implementation of algorithm */
/*  BSOLVE in [1] using complete pivoting in the LU factorization. */

/*  [1] Bo Kagstrom and Lars Westin, */
/*      Generalized Schur Methods with Condition Estimators for */
/*      Solving the Generalized Sylvester Equation, IEEE Transactions */
/*      on Automatic Control, Vol. 34, No. 7, July 1989, pp 745-751. */

/*  [2] Peter Poromaa, */
/*      On Efficient and Robust Estimators for the Separation */
/*      between two Regular Matrix Pairs with Applications in */
/*      Condition Estimation. Report IMINF-95.05, Departement of */
/*      Computing Science, Umea University, S-901 87 Umea, Sweden, 1995. */

/*  ===================================================================== */

/*     .. Parameters .. */
/*<       INTEGER            MAXDIM >*/
/*<       PARAMETER          ( MAXDIM = 8 ) >*/
/*<       DOUBLE PRECISION   ZERO, ONE >*/
/*<       PARAMETER          ( ZERO = 0.0D+0, ONE = 1.0D+0 ) >*/
/*     .. */
/*     .. Local Scalars .. */
/*<       INTEGER            I, INFO, J, K >*/
/*<       DOUBLE PRECISION   BM, BP, PMONE, SMINU, SPLUS, TEMP >*/
/*     .. */
/*     .. Local Arrays .. */
/*<       INTEGER            IWORK( MAXDIM ) >*/
/*<       DOUBLE PRECISION   WORK( 4*MAXDIM ), XM( MAXDIM ), XP( MAXDIM ) >*/
/*     .. */
/*     .. External Subroutines .. */
/*<    >*/
/*     .. */
/*     .. External Functions .. */
/*<       DOUBLE PRECISION   DASUM, DDOT >*/
/*<       EXTERNAL           DASUM, DDOT >*/
/*     .. */
/*     .. Intrinsic Functions .. */
/*<       INTRINSIC          ABS, SQRT >*/
/*     .. */
/*     .. Executable Statements .. */

/*<       IF( IJOB.NE.2 ) THEN >*/
    /* Parameter adjustments */
    z_dim1 = *ldz;
    z_offset = 1 + z_dim1;
    z__ -= z_offset;
    --rhs;
    --ipiv;
    --jpiv;

    /* Function Body */
    if (*ijob != 2) {

/*        Apply permutations IPIV to RHS */

/*<          CALL DLASWP( 1, RHS, LDZ, 1, N-1, IPIV, 1 ) >*/
        i__1 = *n - 1;
        dlaswp_(&c__1, &rhs[1], ldz, &c__1, &i__1, &ipiv[1], &c__1);

/*        Solve for L-part choosing RHS either to +1 or -1. */

/*<          PMONE = -ONE >*/
        pmone = -1.;

/*<          DO 10 J = 1, N - 1 >*/
        i__1 = *n - 1;
        for (j = 1; j <= i__1; ++j) {
/*<             BP = RHS( J ) + ONE >*/
            bp = rhs[j] + 1.;
/*<             BM = RHS( J ) - ONE >*/
            bm = rhs[j] - 1.;
/*<             SPLUS = ONE >*/
            splus = 1.;

/*           Look-ahead for L-part RHS(1:N-1) = + or -1, SPLUS and */
/*           SMIN computed more efficiently than in BSOLVE [1]. */

/*<             SPLUS = SPLUS + DDOT( N-J, Z( J+1, J ), 1, Z( J+1, J ), 1 ) >*/
            i__2 = *n - j;
            splus += ddot_(&i__2, &z__[j + 1 + j * z_dim1], &c__1, &z__[j + 1
                    + j * z_dim1], &c__1);
/*<             SMINU = DDOT( N-J, Z( J+1, J ), 1, RHS( J+1 ), 1 ) >*/
            i__2 = *n - j;
            sminu = ddot_(&i__2, &z__[j + 1 + j * z_dim1], &c__1, &rhs[j + 1],
                     &c__1);
/*<             SPLUS = SPLUS*RHS( J ) >*/
            splus *= rhs[j];
/*<             IF( SPLUS.GT.SMINU ) THEN >*/
            if (splus > sminu) {
/*<                RHS( J ) = BP >*/
                rhs[j] = bp;
/*<             ELSE IF( SMINU.GT.SPLUS ) THEN >*/
            } else if (sminu > splus) {
/*<                RHS( J ) = BM >*/
                rhs[j] = bm;
/*<             ELSE >*/
            } else {

/*              In this case the updating sums are equal and we can */
/*              choose RHS(J) +1 or -1. The first time this happens */
/*              we choose -1, thereafter +1. This is a simple way to */
/*              get good estimates of matrices like Byers well-known */
/*              example (see [1]). (Not done in BSOLVE.) */

/*<                RHS( J ) = RHS( J ) + PMONE >*/
                rhs[j] += pmone;
/*<                PMONE = ONE >*/
                pmone = 1.;
/*<             END IF >*/
            }

/*           Compute the remaining r.h.s. */

/*<             TEMP = -RHS( J ) >*/
            temp = -rhs[j];
/*<             CALL DAXPY( N-J, TEMP, Z( J+1, J ), 1, RHS( J+1 ), 1 ) >*/
            i__2 = *n - j;
            daxpy_(&i__2, &temp, &z__[j + 1 + j * z_dim1], &c__1, &rhs[j + 1],
                     &c__1);

/*<    10    CONTINUE >*/
/* L10: */
        }

/*        Solve for U-part, look-ahead for RHS(N) = +-1. This is not done */
/*        in BSOLVE and will hopefully give us a better estimate because */
/*        any ill-conditioning of the original matrix is transfered to U */
/*        and not to L. U(N, N) is an approximation to sigma_min(LU). */

/*<          CALL DCOPY( N-1, RHS, 1, XP, 1 ) >*/
        i__1 = *n - 1;
        dcopy_(&i__1, &rhs[1], &c__1, xp, &c__1);
/*<          XP( N ) = RHS( N ) + ONE >*/
        xp[*n - 1] = rhs[*n] + 1.;
/*<          RHS( N ) = RHS( N ) - ONE >*/
        rhs[*n] += -1.;
/*<          SPLUS = ZERO >*/
        splus = 0.;
/*<          SMINU = ZERO >*/
        sminu = 0.;
/*<          DO 30 I = N, 1, -1 >*/
        for (i__ = *n; i__ >= 1; --i__) {
/*<             TEMP = ONE / Z( I, I ) >*/
            temp = 1. / z__[i__ + i__ * z_dim1];
/*<             XP( I ) = XP( I )*TEMP >*/
            xp[i__ - 1] *= temp;
/*<             RHS( I ) = RHS( I )*TEMP >*/
            rhs[i__] *= temp;
/*<             DO 20 K = I + 1, N >*/
            i__1 = *n;
            for (k = i__ + 1; k <= i__1; ++k) {
/*<                XP( I ) = XP( I ) - XP( K )*( Z( I, K )*TEMP ) >*/
                xp[i__ - 1] -= xp[k - 1] * (z__[i__ + k * z_dim1] * temp);
/*<                RHS( I ) = RHS( I ) - RHS( K )*( Z( I, K )*TEMP ) >*/
                rhs[i__] -= rhs[k] * (z__[i__ + k * z_dim1] * temp);
/*<    20       CONTINUE >*/
/* L20: */
            }
/*<             SPLUS = SPLUS + ABS( XP( I ) ) >*/
            splus += (d__1 = xp[i__ - 1], abs(d__1));
/*<             SMINU = SMINU + ABS( RHS( I ) ) >*/
            sminu += (d__1 = rhs[i__], abs(d__1));
/*<    30    CONTINUE >*/
/* L30: */
        }
/*<    >*/
        if (splus > sminu) {
            dcopy_(n, xp, &c__1, &rhs[1], &c__1);
        }

/*        Apply the permutations JPIV to the computed solution (RHS) */

/*<          CALL DLASWP( 1, RHS, LDZ, 1, N-1, JPIV, -1 ) >*/
        i__1 = *n - 1;
        dlaswp_(&c__1, &rhs[1], ldz, &c__1, &i__1, &jpiv[1], &c_n1);

/*        Compute the sum of squares */

/*<          CALL DLASSQ( N, RHS, 1, RDSCAL, RDSUM ) >*/
        dlassq_(n, &rhs[1], &c__1, rdscal, rdsum);

/*<       ELSE >*/
    } else {

/*        IJOB = 2, Compute approximate nullvector XM of Z */

/*<          CALL DGECON( 'I', N, Z, LDZ, ONE, TEMP, WORK, IWORK, INFO ) >*/
        dgecon_("I", n, &z__[z_offset], ldz, &c_b23, &temp, work, iwork, &
                info, (ftnlen)1);
/*<          CALL DCOPY( N, WORK( N+1 ), 1, XM, 1 ) >*/
        dcopy_(n, &work[*n], &c__1, xm, &c__1);

/*        Compute RHS */

/*<          CALL DLASWP( 1, XM, LDZ, 1, N-1, IPIV, -1 ) >*/
        i__1 = *n - 1;
        dlaswp_(&c__1, xm, ldz, &c__1, &i__1, &ipiv[1], &c_n1);
/*<          TEMP = ONE / SQRT( DDOT( N, XM, 1, XM, 1 ) ) >*/
        temp = 1. / sqrt(ddot_(n, xm, &c__1, xm, &c__1));
/*<          CALL DSCAL( N, TEMP, XM, 1 ) >*/
        dscal_(n, &temp, xm, &c__1);
/*<          CALL DCOPY( N, XM, 1, XP, 1 ) >*/
        dcopy_(n, xm, &c__1, xp, &c__1);
/*<          CALL DAXPY( N, ONE, RHS, 1, XP, 1 ) >*/
        daxpy_(n, &c_b23, &rhs[1], &c__1, xp, &c__1);
/*<          CALL DAXPY( N, -ONE, XM, 1, RHS, 1 ) >*/
        daxpy_(n, &c_b37, xm, &c__1, &rhs[1], &c__1);
/*<          CALL DGESC2( N, Z, LDZ, RHS, IPIV, JPIV, TEMP ) >*/
        dgesc2_(n, &z__[z_offset], ldz, &rhs[1], &ipiv[1], &jpiv[1], &temp);
/*<          CALL DGESC2( N, Z, LDZ, XP, IPIV, JPIV, TEMP ) >*/
        dgesc2_(n, &z__[z_offset], ldz, xp, &ipiv[1], &jpiv[1], &temp);
/*<    >*/
        if (dasum_(n, xp, &c__1) > dasum_(n, &rhs[1], &c__1)) {
            dcopy_(n, xp, &c__1, &rhs[1], &c__1);
        }

/*        Compute the sum of squares */

/*<          CALL DLASSQ( N, RHS, 1, RDSCAL, RDSUM ) >*/
        dlassq_(n, &rhs[1], &c__1, rdscal, rdsum);

/*<       END IF >*/
    }

/*<       RETURN >*/
    return 0;

/*     End of DLATDF */

/*<       END >*/
} /* dlatdf_ */

#ifdef __cplusplus
        }
#endif
