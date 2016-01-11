/* blas/zdotc.f -- translated by f2c (version 20050501).
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

/*<       double complex function zdotc(n,zx,incx,zy,incy) >*/
/* Double Complex */ VOID zdotc_(doublecomplex * ret_val, integer *n,
        doublecomplex *zx, integer *incx, doublecomplex *zy, integer *incy)
{
    /* System generated locals */
    integer i__1, i__2;
    doublecomplex z__1, z__2, z__3;

    /* Builtin functions */
    void d_cnjg(doublecomplex *, doublecomplex *);

    /* Local variables */
    integer i__, ix, iy;
    doublecomplex ztemp;


/*     forms the dot product of a vector. */
/*     jack dongarra, 3/11/78. */
/*     modified 12/3/93, array(1) declarations changed to array(*) */

/*<       double complex zx(*),zy(*),ztemp >*/
/*<       integer i,incx,incy,ix,iy,n >*/
/*<       ztemp = (0.0d0,0.0d0) >*/
    /* Parameter adjustments */
    --zy;
    --zx;

    /* Function Body */
    ztemp.r = 0., ztemp.i = 0.;
/*<       zdotc = (0.0d0,0.0d0) >*/
     ret_val->r = 0.,  ret_val->i = 0.;
/*<       if(n.le.0)return >*/
    if (*n <= 0) {
        return ;
    }
/*<       if(incx.eq.1.and.incy.eq.1)go to 20 >*/
    if (*incx == 1 && *incy == 1) {
        goto L20;
    }

/*        code for unequal increments or equal increments */
/*          not equal to 1 */

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
/*<         ztemp = ztemp + dconjg(zx(ix))*zy(iy) >*/
        d_cnjg(&z__3, &zx[ix]);
        i__2 = iy;
        z__2.r = z__3.r * zy[i__2].r - z__3.i * zy[i__2].i, z__2.i = z__3.r *
                zy[i__2].i + z__3.i * zy[i__2].r;
        z__1.r = ztemp.r + z__2.r, z__1.i = ztemp.i + z__2.i;
        ztemp.r = z__1.r, ztemp.i = z__1.i;
/*<         ix = ix + incx >*/
        ix += *incx;
/*<         iy = iy + incy >*/
        iy += *incy;
/*<    10 continue >*/
/* L10: */
    }
/*<       zdotc = ztemp >*/
     ret_val->r = ztemp.r,  ret_val->i = ztemp.i;
/*<       return >*/
    return ;

/*        code for both increments equal to 1 */

/*<    20 do 30 i = 1,n >*/
L20:
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/*<         ztemp = ztemp + dconjg(zx(i))*zy(i) >*/
        d_cnjg(&z__3, &zx[i__]);
        i__2 = i__;
        z__2.r = z__3.r * zy[i__2].r - z__3.i * zy[i__2].i, z__2.i = z__3.r *
                zy[i__2].i + z__3.i * zy[i__2].r;
        z__1.r = ztemp.r + z__2.r, z__1.i = ztemp.i + z__2.i;
        ztemp.r = z__1.r, ztemp.i = z__1.i;
/*<    30 continue >*/
/* L30: */
    }
/*<       zdotc = ztemp >*/
     ret_val->r = ztemp.r,  ret_val->i = ztemp.i;
/*<       return >*/
    return ;
/*<       end >*/
} /* zdotc_ */

#ifdef __cplusplus
        }
#endif
