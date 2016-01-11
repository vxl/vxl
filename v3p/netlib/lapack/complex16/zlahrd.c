/* lapack/complex16/zlahrd.f -- translated by f2c (version 20050501).
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

static doublecomplex c_b1 = {0.,0.};
static doublecomplex c_b2 = {1.,0.};
static integer c__1 = 1;

/*<       SUBROUTINE ZLAHRD( N, K, NB, A, LDA, TAU, T, LDT, Y, LDY ) >*/
/* Subroutine */ int zlahrd_(integer *n, integer *k, integer *nb,
        doublecomplex *a, integer *lda, doublecomplex *tau, doublecomplex *t,
        integer *ldt, doublecomplex *y, integer *ldy)
{
    /* System generated locals */
    integer a_dim1, a_offset, t_dim1, t_offset, y_dim1, y_offset, i__1, i__2,
            i__3;
    doublecomplex z__1;

    /* Local variables */
    integer i__;
    doublecomplex ei;
    extern /* Subroutine */ int zscal_(integer *, doublecomplex *,
            doublecomplex *, integer *), zgemv_(char *, integer *, integer *,
            doublecomplex *, doublecomplex *, integer *, doublecomplex *,
            integer *, doublecomplex *, doublecomplex *, integer *, ftnlen),
            zcopy_(integer *, doublecomplex *, integer *, doublecomplex *,
            integer *), zaxpy_(integer *, doublecomplex *, doublecomplex *,
            integer *, doublecomplex *, integer *), ztrmv_(char *, char *,
            char *, integer *, doublecomplex *, integer *, doublecomplex *,
            integer *, ftnlen, ftnlen, ftnlen), zlarfg_(integer *,
            doublecomplex *, doublecomplex *, integer *, doublecomplex *),
            zlacgv_(integer *, doublecomplex *, integer *);


/*  -- LAPACK auxiliary routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     June 30, 1999 */

/*     .. Scalar Arguments .. */
/*<       INTEGER            K, LDA, LDT, LDY, N, NB >*/
/*     .. */
/*     .. Array Arguments .. */
/*<    >*/
/*     .. */

/*  Purpose */
/*  ======= */

/*  ZLAHRD reduces the first NB columns of a complex general n-by-(n-k+1) */
/*  matrix A so that elements below the k-th subdiagonal are zero. The */
/*  reduction is performed by a unitary similarity transformation */
/*  Q' * A * Q. The routine returns the matrices V and T which determine */
/*  Q as a block reflector I - V*T*V', and also the matrix Y = A * V * T. */

/*  This is an auxiliary routine called by ZGEHRD. */

/*  Arguments */
/*  ========= */

/*  N       (input) INTEGER */
/*          The order of the matrix A. */

/*  K       (input) INTEGER */
/*          The offset for the reduction. Elements below the k-th */
/*          subdiagonal in the first NB columns are reduced to zero. */

/*  NB      (input) INTEGER */
/*          The number of columns to be reduced. */

/*  A       (input/output) COMPLEX*16 array, dimension (LDA,N-K+1) */
/*          On entry, the n-by-(n-k+1) general matrix A. */
/*          On exit, the elements on and above the k-th subdiagonal in */
/*          the first NB columns are overwritten with the corresponding */
/*          elements of the reduced matrix; the elements below the k-th */
/*          subdiagonal, with the array TAU, represent the matrix Q as a */
/*          product of elementary reflectors. The other columns of A are */
/*          unchanged. See Further Details. */

/*  LDA     (input) INTEGER */
/*          The leading dimension of the array A.  LDA >= max(1,N). */

/*  TAU     (output) COMPLEX*16 array, dimension (NB) */
/*          The scalar factors of the elementary reflectors. See Further */
/*          Details. */

/*  T       (output) COMPLEX*16 array, dimension (LDT,NB) */
/*          The upper triangular matrix T. */

/*  LDT     (input) INTEGER */
/*          The leading dimension of the array T.  LDT >= NB. */

/*  Y       (output) COMPLEX*16 array, dimension (LDY,NB) */
/*          The n-by-nb matrix Y. */

/*  LDY     (input) INTEGER */
/*          The leading dimension of the array Y. LDY >= max(1,N). */

/*  Further Details */
/*  =============== */

/*  The matrix Q is represented as a product of nb elementary reflectors */

/*     Q = H(1) H(2) . . . H(nb). */

/*  Each H(i) has the form */

/*     H(i) = I - tau * v * v' */

/*  where tau is a complex scalar, and v is a complex vector with */
/*  v(1:i+k-1) = 0, v(i+k) = 1; v(i+k+1:n) is stored on exit in */
/*  A(i+k+1:n,i), and tau in TAU(i). */

/*  The elements of the vectors v together form the (n-k+1)-by-nb matrix */
/*  V which is needed, with T and Y, to apply the transformation to the */
/*  unreduced part of the matrix, using an update of the form: */
/*  A := (I - V*T*V') * (A - Y*V'). */

/*  The contents of A on exit are illustrated by the following example */
/*  with n = 7, k = 3 and nb = 2: */

/*     ( a   h   a   a   a ) */
/*     ( a   h   a   a   a ) */
/*     ( a   h   a   a   a ) */
/*     ( h   h   a   a   a ) */
/*     ( v1  h   a   a   a ) */
/*     ( v1  v2  a   a   a ) */
/*     ( v1  v2  a   a   a ) */

/*  where a denotes an element of the original matrix A, h denotes a */
/*  modified element of the upper Hessenberg matrix H, and vi denotes an */
/*  element of the vector defining H(i). */

/*  ===================================================================== */

/*     .. Parameters .. */
/*<       COMPLEX*16         ZERO, ONE >*/
/*<    >*/
/*     .. */
/*     .. Local Scalars .. */
/*<       INTEGER            I >*/
/*<       COMPLEX*16         EI >*/
/*     .. */
/*     .. External Subroutines .. */
/*<    >*/
/*     .. */
/*     .. Intrinsic Functions .. */
/*<       INTRINSIC          MIN >*/
/*     .. */
/*     .. Executable Statements .. */

/*     Quick return if possible */

/*<    >*/
    /* Parameter adjustments */
    --tau;
    a_dim1 = *lda;
    a_offset = 1 + a_dim1;
    a -= a_offset;
    t_dim1 = *ldt;
    t_offset = 1 + t_dim1;
    t -= t_offset;
    y_dim1 = *ldy;
    y_offset = 1 + y_dim1;
    y -= y_offset;

    /* Function Body */
    if (*n <= 1) {
        return 0;
    }

/*<       DO 10 I = 1, NB >*/
    i__1 = *nb;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<          IF( I.GT.1 ) THEN >*/
        if (i__ > 1) {

/*           Update A(1:n,i) */

/*           Compute i-th column of A - Y * V' */

/*<             CALL ZLACGV( I-1, A( K+I-1, 1 ), LDA ) >*/
            i__2 = i__ - 1;
            zlacgv_(&i__2, &a[*k + i__ - 1 + a_dim1], lda);
/*<    >*/
            i__2 = i__ - 1;
            z__1.r = -1., z__1.i = -0.;
            zgemv_("No transpose", n, &i__2, &z__1, &y[y_offset], ldy, &a[*k
                    + i__ - 1 + a_dim1], lda, &c_b2, &a[i__ * a_dim1 + 1], &
                    c__1, (ftnlen)12);
/*<             CALL ZLACGV( I-1, A( K+I-1, 1 ), LDA ) >*/
            i__2 = i__ - 1;
            zlacgv_(&i__2, &a[*k + i__ - 1 + a_dim1], lda);

/*           Apply I - V * T' * V' to this column (call it b) from the */
/*           left, using the last column of T as workspace */

/*           Let  V = ( V1 )   and   b = ( b1 )   (first I-1 rows) */
/*                    ( V2 )             ( b2 ) */

/*           where V1 is unit lower triangular */

/*           w := V1' * b1 */

/*<             CALL ZCOPY( I-1, A( K+1, I ), 1, T( 1, NB ), 1 ) >*/
            i__2 = i__ - 1;
            zcopy_(&i__2, &a[*k + 1 + i__ * a_dim1], &c__1, &t[*nb * t_dim1 +
                    1], &c__1);
/*<    >*/
            i__2 = i__ - 1;
            ztrmv_("Lower", "Conjugate transpose", "Unit", &i__2, &a[*k + 1 +
                    a_dim1], lda, &t[*nb * t_dim1 + 1], &c__1, (ftnlen)5, (
                    ftnlen)19, (ftnlen)4);

/*           w := w + V2'*b2 */

/*<    >*/
            i__2 = *n - *k - i__ + 1;
            i__3 = i__ - 1;
            zgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*k + i__ +
                    a_dim1], lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b2, &
                    t[*nb * t_dim1 + 1], &c__1, (ftnlen)19);

/*           w := T'*w */

/*<    >*/
            i__2 = i__ - 1;
            ztrmv_("Upper", "Conjugate transpose", "Non-unit", &i__2, &t[
                    t_offset], ldt, &t[*nb * t_dim1 + 1], &c__1, (ftnlen)5, (
                    ftnlen)19, (ftnlen)8);

/*           b2 := b2 - V2*w */

/*<    >*/
            i__2 = *n - *k - i__ + 1;
            i__3 = i__ - 1;
            z__1.r = -1., z__1.i = -0.;
            zgemv_("No transpose", &i__2, &i__3, &z__1, &a[*k + i__ + a_dim1],
                     lda, &t[*nb * t_dim1 + 1], &c__1, &c_b2, &a[*k + i__ +
                    i__ * a_dim1], &c__1, (ftnlen)12);

/*           b1 := b1 - V1*w */

/*<    >*/
            i__2 = i__ - 1;
            ztrmv_("Lower", "No transpose", "Unit", &i__2, &a[*k + 1 + a_dim1]
                    , lda, &t[*nb * t_dim1 + 1], &c__1, (ftnlen)5, (ftnlen)12,
                     (ftnlen)4);
/*<             CALL ZAXPY( I-1, -ONE, T( 1, NB ), 1, A( K+1, I ), 1 ) >*/
            i__2 = i__ - 1;
            z__1.r = -1., z__1.i = -0.;
            zaxpy_(&i__2, &z__1, &t[*nb * t_dim1 + 1], &c__1, &a[*k + 1 + i__
                    * a_dim1], &c__1);

/*<             A( K+I-1, I-1 ) = EI >*/
            i__2 = *k + i__ - 1 + (i__ - 1) * a_dim1;
            a[i__2].r = ei.r, a[i__2].i = ei.i;
/*<          END IF >*/
        }

/*        Generate the elementary reflector H(i) to annihilate */
/*        A(k+i+1:n,i) */

/*<          EI = A( K+I, I ) >*/
        i__2 = *k + i__ + i__ * a_dim1;
        ei.r = a[i__2].r, ei.i = a[i__2].i;
/*<    >*/
        i__2 = *n - *k - i__ + 1;
/* Computing MIN */
        i__3 = *k + i__ + 1;
        zlarfg_(&i__2, &ei, &a[min(i__3,*n) + i__ * a_dim1], &c__1, &tau[i__])
                ;
/*<          A( K+I, I ) = ONE >*/
        i__2 = *k + i__ + i__ * a_dim1;
        a[i__2].r = 1., a[i__2].i = 0.;

/*        Compute  Y(1:n,i) */

/*<    >*/
        i__2 = *n - *k - i__ + 1;
        zgemv_("No transpose", n, &i__2, &c_b2, &a[(i__ + 1) * a_dim1 + 1],
                lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b1, &y[i__ *
                y_dim1 + 1], &c__1, (ftnlen)12);
/*<    >*/
        i__2 = *n - *k - i__ + 1;
        i__3 = i__ - 1;
        zgemv_("Conjugate transpose", &i__2, &i__3, &c_b2, &a[*k + i__ +
                a_dim1], lda, &a[*k + i__ + i__ * a_dim1], &c__1, &c_b1, &t[
                i__ * t_dim1 + 1], &c__1, (ftnlen)19);
/*<    >*/
        i__2 = i__ - 1;
        z__1.r = -1., z__1.i = -0.;
        zgemv_("No transpose", n, &i__2, &z__1, &y[y_offset], ldy, &t[i__ *
                t_dim1 + 1], &c__1, &c_b2, &y[i__ * y_dim1 + 1], &c__1, (
                ftnlen)12);
/*<          CALL ZSCAL( N, TAU( I ), Y( 1, I ), 1 ) >*/
        zscal_(n, &tau[i__], &y[i__ * y_dim1 + 1], &c__1);

/*        Compute T(1:i,i) */

/*<          CALL ZSCAL( I-1, -TAU( I ), T( 1, I ), 1 ) >*/
        i__2 = i__ - 1;
        i__3 = i__;
        z__1.r = -tau[i__3].r, z__1.i = -tau[i__3].i;
        zscal_(&i__2, &z__1, &t[i__ * t_dim1 + 1], &c__1);
/*<    >*/
        i__2 = i__ - 1;
        ztrmv_("Upper", "No transpose", "Non-unit", &i__2, &t[t_offset], ldt,
                &t[i__ * t_dim1 + 1], &c__1, (ftnlen)5, (ftnlen)12, (ftnlen)8)
                ;
/*<          T( I, I ) = TAU( I ) >*/
        i__2 = i__ + i__ * t_dim1;
        i__3 = i__;
        t[i__2].r = tau[i__3].r, t[i__2].i = tau[i__3].i;

/*<    10 CONTINUE >*/
/* L10: */
    }
/*<       A( K+NB, NB ) = EI >*/
    i__1 = *k + *nb + *nb * a_dim1;
    a[i__1].r = ei.r, a[i__1].i = ei.i;

/*<       RETURN >*/
    return 0;

/*     End of ZLAHRD */

/*<       END >*/
} /* zlahrd_ */

#ifdef __cplusplus
        }
#endif
