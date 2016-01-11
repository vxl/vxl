/* lapack/complex16/zgehd2.f -- translated by f2c (version 20050501).
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

/*<       SUBROUTINE ZGEHD2( N, ILO, IHI, A, LDA, TAU, WORK, INFO ) >*/
/* Subroutine */ int zgehd2_(integer *n, integer *ilo, integer *ihi,
        doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *
        work, integer *info)
{
    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    doublecomplex z__1;

    /* Builtin functions */
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__;
    doublecomplex alpha;
    extern /* Subroutine */ int zlarf_(char *, integer *, integer *,
            doublecomplex *, integer *, doublecomplex *, doublecomplex *,
            integer *, doublecomplex *, ftnlen), xerbla_(char *, integer *,
            ftnlen), zlarfg_(integer *, doublecomplex *, doublecomplex *,
            integer *, doublecomplex *);


/*  -- LAPACK routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     September 30, 1994 */

/*     .. Scalar Arguments .. */
/*<       INTEGER            IHI, ILO, INFO, LDA, N >*/
/*     .. */
/*     .. Array Arguments .. */
/*<       COMPLEX*16         A( LDA, * ), TAU( * ), WORK( * ) >*/
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZGEHD2 reduces a complex general matrix A to upper Hessenberg form H */
/*  by a unitary similarity transformation:  Q' * A * Q = H . */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A.  N >= 0. */

/*  ILO     (input) INTEGER */
/*  IHI     (input) INTEGER */
/*          It is assumed that A is already upper triangular in rows */
/*          and columns 1:ILO-1 and IHI+1:N. ILO and IHI are normally */
/*          set by a previous call to ZGEBAL; otherwise they should be */
/*          set to 1 and N respectively. See Further Details. */
/*          1 <= ILO <= IHI <= max(1,N). */

/*  A       (input/output) COMPLEX*16 array, dimension (LDA,N) */
/*          On entry, the n by n general matrix to be reduced. */
/*          On exit, the upper triangle and the first subdiagonal of A */
/*          are overwritten with the upper Hessenberg matrix H, and the */
/*          elements below the first subdiagonal, with the array TAU, */
/*          represent the unitary matrix Q as a product of elementary */
/*          reflectors. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  TAU     (output) COMPLEX*16 array, dimension (N-1) */
/*          The scalar factors of the elementary reflectors (see Further */
/*          Details). */

/*  WORK    (workspace) COMPLEX*16 array, dimension (N) */

/*  INFO    (output) INTEGER */
/*          = 0:  successful exit */
/*          < 0:  if INFO = -i, the i-th argument had an illegal value. */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of (ihi-ilo) elementary */
/*  reflectors */

/*     Q = H(ilo) H(ilo+1) . . . H(ihi-1). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a complex scalar, and v is a complex vector with */
/*  v(1:i) = 0, v(i+1) = 1 and v(ihi+1:n) = 0; v(i+2:ihi) is stored on */
/*  exit in A(i+2:ihi,i), and tau in TAU(i). */

/*  The contents of A are illustrated by the following example, with */
/*  n = 7, ilo = 2 and ihi = 6: */

/*  on entry,                        on exit, */

/*  ( a   a   a   a   a   a   a )    (  a   a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      a   h   h   h   h   a ) */
/*  (     a   a   a   a   a   a )    (      h   h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  h   h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  h   h   h   h ) */
/*  (     a   a   a   a   a   a )    (      v2  v3  v4  h   h   h ) */
/*  (                         a )    (                          a ) */

/*  where a denotes an element of the original matrix A, h denotes a */
/*  modified element of the upper Hessenberg matrix H, and vi denotes an */
/*  element of the vector defining H(i). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*<       COMPLEX*16         ONE >*/
/*<       PARAMETER          ( ONE = ( 1.0D+0, 0.0D+0 ) ) >*/
/*     .. */
/*     .. Local Scalars .. */
/*<       INTEGER            I >*/
/*<       COMPLEX*16         ALPHA >*/
/*     .. */
/*     .. External Subroutines .. */
/*<       EXTERNAL           XERBLA, ZLARF, ZLARFG >*/
/*     .. */
/*     .. Intrinsic Functions .. */
/*<       INTRINSIC          DCONJG, MAX, MIN >*/
/*     .. */
/*     .. Executable Statements .. */

/*     Test the input parameters */

/*<       INFO = 0 >*/
    /* Parameter adjustments */
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    --tau;
    --work;

    /* Function Body */
    *info = 0;
/*<       IF( N.LT.0 ) THEN >*/
    if (*n < 0) {
/*<          INFO = -1 >*/
        *info = -1;
/*<       ELSE IF( ILO.LT.1 .OR. ILO.GT.MAX( 1, N ) ) THEN >*/
    } else if (*ilo < 1 || *ilo > max(1,*n)) {
/*<          INFO = -2 >*/
        *info = -2;
/*<       ELSE IF( IHI.LT.MIN( ILO, N ) .OR. IHI.GT.N ) THEN >*/
    } else if (*ihi < min(*ilo,*n) || *ihi > *n) {
/*<          INFO = -3 >*/
        *info = -3;
/*<       ELSE IF( LDA.LT.MAX( 1, N ) ) THEN >*/
    } else if (*lda < max(1,*n)) {
/*<          INFO = -5 >*/
        *info = -5;
/*<       END IF >*/
    }
/*<       IF( INFO.NE.0 ) THEN >*/
    if (*info != 0) {
/*<          CALL XERBLA( 'ZGEHD2', -INFO ) >*/
        i__1 = -(*info);
        xerbla_("ZGEHD2", &i__1, (ftnlen)6);
/*<          RETURN >*/
        return 0;
/*<       END IF >*/
    }

/*<       DO 10 I = ILO, IHI - 1 >*/
    i__1 = *ihi - 1;
    for (i__ = *ilo; i__ <= i__1; ++i__) {

/*        Compute elementary reflector H(i) to annihilate A(i+2:ihi,i) */

/*<          ALPHA = A( I+1, I ) >*/
        i__2 = i__ + 1 + i__ * a_dim1;
        alpha.r = a[i__2].r, alpha.i = a[i__2].i;
/*<          CALL ZLARFG( IHI-I, ALPHA, A( MIN( I+2, N ), I ), 1, TAU( I ) ) >*/
        i__2 = *ihi - i__;
/* Computing MIN */
        i__3 = i__ + 2;
        zlarfg_(&i__2, &alpha, &a[min(i__3,*n) + i__ * a_dim1], &c__1, &tau[
                i__]);
/*<          A( I+1, I ) = ONE >*/
        i__2 = i__ + 1 + i__ * a_dim1;
        a[i__2].r = 1., a[i__2].i = 0.;

/*        Apply H(i) to A(1:ihi,i+1:ihi) from the right */

/*<    >*/
        i__2 = *ihi - i__;
        zlarf_("Right", ihi, &i__2, &a[i__ + 1 + i__ * a_dim1], &c__1, &tau[
                i__], &a[(i__ + 1) * a_dim1 + 1], lda, &work[1], (ftnlen)5);

/*        Apply H(i)' to A(i+1:ihi,i+1:n) from the left */

/*<    >*/
        i__2 = *ihi - i__;
        i__3 = *n - i__;
        d_cnjg(&z__1, &tau[i__]);
        zlarf_("Left", &i__2, &i__3, &a[i__ + 1 + i__ * a_dim1], &c__1, &z__1,
                 &a[i__ + 1 + (i__ + 1) * a_dim1], lda, &work[1], (ftnlen)4);

/*<          A( I+1, I ) = ALPHA >*/
        i__2 = i__ + 1 + i__ * a_dim1;
        a[i__2].r = alpha.r, a[i__2].i = alpha.i;
/*<    10 CONTINUE >*/
/* L10: */
    }

/*<       RETURN >*/
    return 0;

/*     End of ZGEHD2 */

/*<       END >*/
} /* zgehd2_ */

#ifdef __cplusplus
        }
#endif
