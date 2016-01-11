/* lapack/double/dlarf.f -- translated by f2c (version 20050501).
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

static doublereal c_b4 = 1.;
static doublereal c_b5 = 0.;
static integer c__1 = 1;

/*<       SUBROUTINE DLARF( SIDE, M, N, V, INCV, TAU, C, LDC, WORK ) >*/
/* Subroutine */ int dlarf_(char *side, integer *m, integer *n, doublereal *v,
         integer *incv, doublereal *tau, doublereal *c__, integer *ldc,
        doublereal *work, ftnlen side_len)
{
    /* System generated locals */
    integer c_dim1, c_offset;
    doublereal d__1;

    /* Local variables */
    extern /* Subroutine */ int dger_(integer *, integer *, doublereal *,
            doublereal *, integer *, doublereal *, integer *, doublereal *,
            integer *);
    extern logical lsame_(const char *, const char *, ftnlen, ftnlen);
    extern /* Subroutine */ int dgemv_(char *, integer *, integer *,
            doublereal *, doublereal *, integer *, doublereal *, integer *,
            doublereal *, doublereal *, integer *, ftnlen);
    (void)side_len;

/*  -- LAPACK auxiliary routine (version 3.0) -- */
/*     Univ. of Tennessee, Univ. of California Berkeley, NAG Ltd., */
/*     Courant Institute, Argonne National Lab, and Rice University */
/*     February 29, 1992 */

/*     .. Scalar Arguments .. */
/*<       CHARACTER          SIDE >*/
/*<       INTEGER            INCV, LDC, M, N >*/
/*<       DOUBLE PRECISION   TAU >*/
/*     .. */
/*     .. Array Arguments .. */
/*<       DOUBLE PRECISION   C( LDC, * ), V( * ), WORK( * ) >*/
/*     .. */

/*  Purpose */
/*  ======= */

/*  DLARF applies a real elementary reflector H to a real m by n matrix */
/*  C, from either the left or the right. H is represented in the form */

/*        H = I - tau * v * v' */

/*  where tau is a real scalar and v is a real vector. */

/*  If tau = 0, then H is taken to be the unit matrix. */

/*  Arguments */
/*  ========= */

/*  SIDE    (input) CHARACTER*1 */
/*          = 'L': form  H * C */
/*          = 'R': form  C * H */

/*  M       (input) INTEGER */
/*          The number of rows of the matrix C. */

/*  N       (input) INTEGER */
/*          The number of columns of the matrix C. */

/*  V       (input) DOUBLE PRECISION array, dimension */
/*                     (1 + (M-1)*abs(INCV)) if SIDE = 'L' */
/*                  or (1 + (N-1)*abs(INCV)) if SIDE = 'R' */
/*          The vector v in the representation of H. V is not used if */
/*          TAU = 0. */

/*  INCV    (input) INTEGER */
/*          The increment between elements of v. INCV <> 0. */

/*  TAU     (input) DOUBLE PRECISION */
/*          The value tau in the representation of H. */

/*  C       (input/output) DOUBLE PRECISION array, dimension (LDC,N) */
/*          On entry, the m by n matrix C. */
/*          On exit, C is overwritten by the matrix H * C if SIDE = 'L', */
/*          or C * H if SIDE = 'R'. */

/*  LDC     (input) INTEGER */
/*          The leading dimension of the array C. LDC >= max(1,M). */

/*  WORK    (workspace) DOUBLE PRECISION array, dimension */
/*                         (N) if SIDE = 'L' */
/*                      or (M) if SIDE = 'R' */

/*  ===================================================================== */

/*     .. Parameters .. */
/*<       DOUBLE PRECISION   ONE, ZERO >*/
/*<       PARAMETER          ( ONE = 1.0D+0, ZERO = 0.0D+0 ) >*/
/*     .. */
/*     .. External Subroutines .. */
/*<       EXTERNAL           DGEMV, DGER >*/
/*     .. */
/*     .. External Functions .. */
/*<       LOGICAL            LSAME >*/
/*<       EXTERNAL           LSAME >*/
/*     .. */
/*     .. Executable Statements .. */

/*<       IF( LSAME( SIDE, 'L' ) ) THEN >*/
    /* Parameter adjustments */
    --v;
    c_dim1 = *ldc;
    c_offset = 1 + c_dim1;
    c__ -= c_offset;
    --work;

    /* Function Body */
    if (lsame_(side, "L", (ftnlen)1, (ftnlen)1)) {

/*        Form  H * C */

/*<          IF( TAU.NE.ZERO ) THEN >*/
        if (*tau != 0.) {

/*           w := C' * v */

/*<    >*/
            dgemv_("Transpose", m, n, &c_b4, &c__[c_offset], ldc, &v[1], incv,
                     &c_b5, &work[1], &c__1, (ftnlen)9);

/*           C := C - v * w' */

/*<             CALL DGER( M, N, -TAU, V, INCV, WORK, 1, C, LDC ) >*/
            d__1 = -(*tau);
            dger_(m, n, &d__1, &v[1], incv, &work[1], &c__1, &c__[c_offset],
                    ldc);
/*<          END IF >*/
        }
/*<       ELSE >*/
    } else {

/*        Form  C * H */

/*<          IF( TAU.NE.ZERO ) THEN >*/
        if (*tau != 0.) {

/*           w := C * v */

/*<    >*/
            dgemv_("No transpose", m, n, &c_b4, &c__[c_offset], ldc, &v[1],
                    incv, &c_b5, &work[1], &c__1, (ftnlen)12);

/*           C := C - w * v' */

/*<             CALL DGER( M, N, -TAU, WORK, 1, V, INCV, C, LDC ) >*/
            d__1 = -(*tau);
            dger_(m, n, &d__1, &work[1], &c__1, &v[1], incv, &c__[c_offset],
                    ldc);
/*<          END IF >*/
        }
/*<       END IF >*/
    }
/*<       RETURN >*/
    return 0;

/*     End of DLARF */

/*<       END >*/
} /* dlarf_ */

#ifdef __cplusplus
        }
#endif
