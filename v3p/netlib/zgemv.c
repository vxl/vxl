/*  -- translated by f2c (version of 23 April 1993  18:34:30).
   You must link the resulting object file with the libraries:
        -lf2c -lm   (in that order)
*/

#include "f2c.h"

/* Modified by Peter Vanroose, June 2001: manual optimisation and clean-up */

/* Subroutine */ void zgemv_(trans, m, n, alpha, a, lda, x, incx, beta, y, incy, trans_len)
char *trans;
integer *m, *n;
doublecomplex *alpha, *a;
integer *lda;
doublecomplex *x;
integer *incx;
doublecomplex *beta, *y;
integer *incy;
ftnlen trans_len;
{
    /* System generated locals */
    integer i__1;
    doublecomplex z__1;

    /* Builtin functions */
    void d_cnjg();

    /* Local variables */
    static integer info;
    static doublecomplex temp;
    static integer lenx, leny, i, j;
    extern logical lsame_();
    static integer ix, iy, jx, jy, kx, ky;
    extern /* Subroutine */ void xerbla_();
    static logical noconj;

/*  ===================================================================== */
/*                                                                        */
/*  Purpose                                                               */
/*  =======                                                               */
/*                                                                        */
/*  ZGEMV  performs one of the matrix-vector operations                   */
/*                                                                        */
/*     y := alpha*A*x + beta*y,   or   y := alpha*A'*x + beta*y,   or     */
/*                                                                        */
/*     y := alpha*conjg( A' )*x + beta*y,                                 */
/*                                                                        */
/*  where alpha and beta are scalars, x and y are vectors and A is an     */
/*  m by n matrix.                                                        */
/*                                                                        */
/*  Parameters                                                            */
/*  ==========                                                            */
/*                                                                        */
/*  TRANS  - CHARACTER*1.                                                 */
/*           On entry, TRANS specifies the operation to be performed as   */
/*           follows:                                                     */
/*                                                                        */
/*              TRANS = 'N' or 'n'   y := alpha*A*x + beta*y.             */
/*                                                                        */
/*              TRANS = 'T' or 't'   y := alpha*A'*x + beta*y.            */
/*                                                                        */
/*              TRANS = 'C' or 'c'   y := alpha*conjg( A' )*x + beta*y.   */
/*                                                                        */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  M      - INTEGER.                                                     */
/*           On entry, M specifies the number of rows of the matrix A.    */
/*           M must be at least zero.                                     */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  N      - INTEGER.                                                     */
/*           On entry, N specifies the number of columns of the matrix A. */
/*           N must be at least zero.                                     */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  ALPHA  - COMPLEX*16      .                                            */
/*           On entry, ALPHA specifies the scalar alpha.                  */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  A      - COMPLEX*16       array of DIMENSION ( LDA, n ).              */
/*           Before entry, the leading m by n part of the array A must    */
/*           contain the matrix of coefficients.                          */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  LDA    - INTEGER.                                                     */
/*           On entry, LDA specifies the first dimension of A as declared */
/*           in the calling (sub) program. LDA must be at least           */
/*           max( 1, m ).                                                 */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  X      - COMPLEX*16       array of DIMENSION at least                 */
/*           ( 1 + ( n - 1 )*abs( INCX ) ) when TRANS = 'N' or 'n'        */
/*           and at least                                                 */
/*           ( 1 + ( m - 1 )*abs( INCX ) ) otherwise.                     */
/*           Before entry, the incremented array X must contain the       */
/*           vector x.                                                    */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  INCX   - INTEGER.                                                     */
/*           On entry, INCX specifies the increment for the elements of   */
/*           X. INCX must not be zero.                                    */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  BETA   - COMPLEX*16      .                                            */
/*           On entry, BETA specifies the scalar beta. When BETA is       */
/*           supplied as zero then Y need not be set on input.            */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  Y      - COMPLEX*16       array of DIMENSION at least                 */
/*           ( 1 + ( m - 1 )*abs( INCY ) ) when TRANS = 'N' or 'n'        */
/*           and at least                                                 */
/*           ( 1 + ( n - 1 )*abs( INCY ) ) otherwise.                     */
/*           Before entry with BETA non-zero, the incremented array Y     */
/*           must contain the vector y. On exit, Y is overwritten by the  */
/*           updated vector y.                                            */
/*                                                                        */
/*  INCY   - INTEGER.                                                     */
/*           On entry, INCY specifies the increment for the elements of   */
/*           Y. INCY must not be zero.                                    */
/*           Unchanged on exit.                                           */
/*                                                                        */
/*  ===================================================================== */

/*  Level 2 Blas routine. */

/*  -- Written on 22-October-1986. */
/*     Jack Dongarra, Argonne National Lab. */
/*     Jeremy Du Croz, Nag Central Office. */
/*     Sven Hammarling, Nag Central Office. */
/*     Richard Hanson, Sandia National Labs. */

    /* Function Body */

    info = 0;
    if (! lsame_(trans, "N", 1L, 1L) && ! lsame_(trans, "T", 1L, 1L) && !
            lsame_(trans, "C", 1L, 1L)) {
        info = 1;
    } else if (*m < 0) {
        info = 2;
    } else if (*n < 0) {
        info = 3;
    } else if (*lda < max(1,*m)) {
        info = 6;
    } else if (*incx == 0) {
        info = 8;
    } else if (*incy == 0) {
        info = 11;
    }
    if (info != 0) {
        xerbla_("ZGEMV ", &info, 6L);
        return;
    }

/*     Quick return if possible. */

    if (*m == 0 || *n == 0 || alpha->r == 0. && alpha->i == 0. && (beta->r ==
            1. && beta->i == 0.)) {
        return;
    }

    noconj = lsame_(trans, "T", 1L, 1L);

/*     Set  LENX  and  LENY, the lengths of the vectors x and y, and set */
/*     up the start points in  X  and  Y. */

    if (lsame_(trans, "N", 1L, 1L)) {
        lenx = *n;
        leny = *m;
    } else {
        lenx = *m;
        leny = *n;
    }
    if (*incx > 0) {
        kx = 0;
    } else {
        kx = (1-lenx) * *incx;
    }
    if (*incy > 0) {
        ky = 0;
    } else {
        ky = (1-leny) * *incy;
    }

/*     Start the operations. In this version the elements of A are */
/*     accessed sequentially with one pass through A. */

/*     First form  y := beta*y. */

    if (beta->r != 1. || beta->i != 0.) {
        if (*incy == 1) {
            if (beta->r == 0. && beta->i == 0.) {
                for (i = 0; i < leny; ++i) {
                    y[i].r = 0., y[i].i = 0.;
                }
            } else {
                for (i = 0; i < leny; ++i) {
                    z__1.r = beta->r * y[i].r - beta->i * y[i].i,
                    z__1.i = beta->r * y[i].i + beta->i * y[i].r;
                    y[i].r = z__1.r, y[i].i = z__1.i;
                }
            }
        } else {
            iy = ky;
            if (beta->r == 0. && beta->i == 0.) {
                for (i = 0; i < leny; ++i) {
                    y[iy].r = 0., y[iy].i = 0.;
                    iy += *incy;
                }
            } else {
                for (i = 0; i < leny; ++i) {
                    z__1.r = beta->r * y[iy].r - beta->i * y[iy].i,
                    z__1.i = beta->r * y[iy].i + beta->i * y[iy].r;
                    y[iy].r = z__1.r, y[iy].i = z__1.i;
                    iy += *incy;
                }
            }
        }
    }
    if (alpha->r == 0. && alpha->i == 0.) {
        return;
    }
    if (lsame_(trans, "N", 1L, 1L)) {

/*        Form  y := alpha*A*x + y. */

        jx = kx;
        if (*incy == 1) {
            for (j = 0; j < *n; ++j) {
                if (x[jx].r != 0. || x[jx].i != 0.) {
                    temp.r = alpha->r * x[jx].r - alpha->i * x[jx].i,
                    temp.i = alpha->r * x[jx].i + alpha->i * x[jx].r;
                    for (i = 0; i < *m; ++i) {
                        i__1 = i + j * *lda;
                        y[i].r += temp.r * a[i__1].r - temp.i * a[i__1].i,
                        y[i].i += temp.r * a[i__1].i + temp.i * a[i__1].r;
                    }
                }
                jx += *incx;
            }
        } else {
            for (j = 0; j < *n; ++j) {
                if (x[jx].r != 0. || x[jx].i != 0.) {
                    temp.r = alpha->r * x[jx].r - alpha->i * x[jx].i,
                    temp.i = alpha->r * x[jx].i + alpha->i * x[jx].r;
                    iy = ky;
                    for (i = 0; i < *m; ++i) {
                        i__1 = i + j * *lda;
                        y[iy].r += temp.r * a[i__1].r - temp.i * a[i__1].i,
                        y[iy].i += temp.r * a[i__1].i + temp.i * a[i__1].r;
                        iy += *incy;
                    }
                }
                jx += *incx;
            }
        }
    } else {

/*        Form  y := alpha*A'*x + y  or  y := alpha*conjg( A' )*x + y.  */

        jy = ky;
        if (*incx == 1) {
            for (j = 0; j < *n; ++j) {
                temp.r = 0., temp.i = 0.;
                if (noconj) {
                    for (i = 0; i < *m; ++i) {
                        i__1 = i + j * *lda;
                        temp.r += a[i__1].r * x[i].r - a[i__1].i * x[i].i,
                        temp.i += a[i__1].r * x[i].i + a[i__1].i * x[i].r;
                    }
                } else {
                    for (i = 0; i < *m; ++i) {
                        d_cnjg(&z__1, &a[i + j * *lda]);
                        temp.r += z__1.r * x[i].r - z__1.i * x[i].i,
                        temp.i += z__1.r * x[i].i + z__1.i * x[i].r;
                    }
                }
                y[jy].r += alpha->r * temp.r - alpha->i * temp.i,
                y[jy].i += alpha->r * temp.i + alpha->i * temp.r;
                jy += *incy;
            }
        } else {
            for (j = 0; j < *n; ++j) {
                temp.r = 0., temp.i = 0.;
                ix = kx;
                if (noconj) {
                    for (i = 0; i < *m; ++i) {
                        i__1 = i + j * *lda;
                        temp.r += a[i__1].r * x[ix].r - a[i__1].i * x[ix].i,
                        temp.i += a[i__1].r * x[ix].i + a[i__1].i * x[ix].r;
                        ix += *incx;
                    }
                } else {
                    for (i = 0; i < *m; ++i) {
                        d_cnjg(&z__1, &a[i + j * *lda]);
                        temp.r += z__1.r * x[ix].r - z__1.i * x[ix].i,
                        temp.i += z__1.r * x[ix].i + z__1.i * x[ix].r;
                        ix += *incx;
                    }
                }
                y[jy].r += alpha->r * temp.r - alpha->i * temp.i,
                y[jy].i += alpha->r * temp.i + alpha->i * temp.r;
                jy += *incy;
            }
        }
    }

/*     End of ZGEMV . */

} /* zgemv_ */

