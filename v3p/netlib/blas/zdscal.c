/* blas/zdscal.f -- translated by f2c (version 20050501).
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

/*<       subroutine  zdscal(n,da,zx,incx) >*/
/* Subroutine */ int zdscal_(integer *n, doublereal *da, doublecomplex *zx,
        integer *incx)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    doublecomplex z__1, z__2;

    /* Local variables */
    integer i__, ix;


/*     scales a vector by a constant. */
/*     jack dongarra, 3/11/78. */
/*     modified 3/93 to return if incx .le. 0. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

/*<       double complex zx(*) >*/
/*<       double precision da >*/
/*<       integer i,incx,ix,n >*/

/*<       if( n.le.0 .or. incx.le.0 )return >*/
    /* Parameter adjustments */
    --zx;

    /* Function Body */
    if (*n <= 0 || *incx <= 0) {
        return 0;
    }
/*<       if(incx.eq.1)go to 20 >*/
    if (*incx == 1) {
        goto L20;
    }

/*        code for increment not equal to 1 */

/*<       ix = 1 >*/
    ix = 1;
/*<       do 10 i = 1,n >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         zx(ix) = dcmplx(da,0.0d0)*zx(ix) >*/
        i__2 = ix;
        z__2.r = *da, z__2.i = 0.;
        i__3 = ix;
        z__1.r = z__2.r * zx[i__3].r - z__2.i * zx[i__3].i, z__1.i = z__2.r *
                zx[i__3].i + z__2.i * zx[i__3].r;
        zx[i__2].r = z__1.r, zx[i__2].i = z__1.i;
/*<         ix = ix + incx >*/
        ix += *incx;
/*<    10 continue >*/
/* L10: */
    }
/*<       return >*/
    return 0;

/*        code for increment equal to 1 */

/*<    20 do 30 i = 1,n >*/
L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         zx(i) = dcmplx(da,0.0d0)*zx(i) >*/
        i__2 = i__;
        z__2.r = *da, z__2.i = 0.;
        i__3 = i__;
        z__1.r = z__2.r * zx[i__3].r - z__2.i * zx[i__3].i, z__1.i = z__2.r *
                zx[i__3].i + z__2.i * zx[i__3].r;
        zx[i__2].r = z__1.r, zx[i__2].i = z__1.i;
/*<    30 continue >*/
/* L30: */
    }
/*<       return >*/
    return 0;
/*<       end >*/
} /* zdscal_ */

#ifdef __cplusplus
        }
#endif
