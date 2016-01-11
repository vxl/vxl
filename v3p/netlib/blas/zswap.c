/* blas/zswap.f -- translated by f2c (version 20050501).
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

/*<       subroutine  zswap (n,zx,incx,zy,incy) >*/
/* Subroutine */ int zswap_(integer *n, doublecomplex *zx, integer *incx,
        doublecomplex *zy, integer *incy)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    integer i__, ix, iy;
    doublecomplex ztemp;


/*     interchanges two vectors. */
/*     jack dongarra, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

/*<       double complex zx(*),zy(*),ztemp >*/
/*<       integer i,incx,incy,ix,iy,n >*/

/*<       if(n.le.0)return >*/
    /* Parameter adjustments */
    --zy;
    --zx;

    /* Function Body */
    if (*n <= 0) {
        return 0;
    }
/*<       if(incx.eq.1.and.incy.eq.1)go to 20 >*/
    if (*incx == 1 && *incy == 1) {
        goto L20;
    }

/*       code for unequal increments or equal increments not equal */
/*         to 1 */

/*<       ix = 1 >*/
    ix = 1;
/*<       iy = 1 >*/
    iy = 1;
/*<       if(incx.lt.0)ix = (-n+1)*incx + 1 >*/
    if (*incx < 0) {
        ix = (-(*n) + 1) * *incx + 1;
    }
/*<       if(incy.lt.0)iy = (-n+1)*incy + 1 >*/
    if (*incy < 0) {
        iy = (-(*n) + 1) * *incy + 1;
    }
/*<       do 10 i = 1,n >*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         ztemp = zx(ix) >*/
        i__2 = ix;
        ztemp.r = zx[i__2].r, ztemp.i = zx[i__2].i;
/*<         zx(ix) = zy(iy) >*/
        i__2 = ix;
        i__3 = iy;
        zx[i__2].r = zy[i__3].r, zx[i__2].i = zy[i__3].i;
/*<         zy(iy) = ztemp >*/
        i__2 = iy;
        zy[i__2].r = ztemp.r, zy[i__2].i = ztemp.i;
/*<         ix = ix + incx >*/
        ix += *incx;
/*<         iy = iy + incy >*/
        iy += *incy;
/*<    10 continue >*/
/* L10: */
    }
/*<       return >*/
    return 0;

/*       code for both increments equal to 1 */
/*<    20 do 30 i = 1,n >*/
L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         ztemp = zx(i) >*/
        i__2 = i__;
        ztemp.r = zx[i__2].r, ztemp.i = zx[i__2].i;
/*<         zx(i) = zy(i) >*/
        i__2 = i__;
        i__3 = i__;
        zx[i__2].r = zy[i__3].r, zx[i__2].i = zy[i__3].i;
/*<         zy(i) = ztemp >*/
        i__2 = i__;
        zy[i__2].r = ztemp.r, zy[i__2].i = ztemp.i;
/*<    30 continue >*/
/* L30: */
    }
/*<       return >*/
    return 0;
/*<       end >*/
} /* zswap_ */

#ifdef __cplusplus
        }
#endif
