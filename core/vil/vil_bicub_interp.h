// This is core/vil2/vil2_bicub_interp.h
#ifndef vil2_bicub_interp_h_
#define vil2_bicub_interp_h_
//:
// \file
// \brief Bicubic interpolation functions for 2D images

// The vil2 bicub source files were derived from the corresponding
// vil2 bilin files, thus the vil2 bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin/bicub file that would likely also benefit from
// the same change.

#include <vcl_cassert.h>
#include <vcl_cstddef.h>

//: Compute bicubic interpolation at (x,y), no bound checks. Requires 1<x<ni-3, 1<y<nj-3
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
inline double vil2_bicub_interp_unsafe(double x, double y, const T* data,
                                       vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    int p1x=int(x);
    double normx = x-p1x;
    int p1y=int(y);
    double normy = y-p1y;

    const T* pix1 = data + p1y*ystep + p1x*xstep;

    // like bilinear interpolation, use separability.
    // the s's are for the x-direction and the t's for the y-direction.
    double s0 = ((2-normx)*normx-1)*normx;    // -1
    double s1 = (3*normx-5)*normx*normx+2;    //  0
    double s2 = ((4-3*normx)*normx+1)*normx;  // +1
    double s3 = (normx-1)*normx*normx;        // +2

    double t0 = ((2-normy)*normy-1)*normy;
    double t1 = (3*normy-5)*normy*normy+2;
    double t2 = ((4-3*normy)*normy+1)*normy;
    double t3 = (normy-1)*normy*normy;

#define vil2_I(dx,dy) (pix1[(dx)*xstep+(dy)*ystep])

    double val = 0.25*
                 ( (s0*vil2_I(-1,-1) + s1*vil2_I(+0,-1) + s2*vil2_I(+1,-1) + s3*vil2_I(+2,-1))*t0 +
                   (s0*vil2_I(-1,+0) + s1*vil2_I(+0,+0) + s2*vil2_I(+1,+0) + s3*vil2_I(+2,+0))*t1 +
                   (s0*vil2_I(-1,+1) + s1*vil2_I(+0,+1) + s2*vil2_I(+1,+1) + s3*vil2_I(+2,+1))*t2 +
                   (s0*vil2_I(-1,+2) + s1*vil2_I(+0,+2) + s2*vil2_I(+1,+2) + s3*vil2_I(+2,+2))*t3 );

#undef vil2_I

    return val;
}
//: Compute bicubic interpolation at (x,y), no bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
inline double vil2_bicub_interp_raw(double x, double y, const T* data,
                                    vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    int p1x=int(x);
    double normx = x-p1x;
    int p1y=int(y);
    double normy = y-p1y;

    const T* pix1 = data + p1y*ystep + p1x*xstep;

    // special boundary cases can be handled more quickly first;
    // also avoids accessing an invalid pix1[t] which is going to have weight 0.
    if (normx == 0 && normy == 0) return pix1[0];

    // like bilinear interpolation, use separability.
    // the s's are for the x-direction and the t's for the y-direction.
    double s0 = ((2-normx)*normx-1)*normx;    // -1
    double s1 = (3*normx-5)*normx*normx+2;    //  0
    double s2 = ((4-3*normx)*normx+1)*normx;  // +1
    double s3 = (normx-1)*normx*normx;        // +2

#define vil2_I(dx,dy) (pix1[(dx)*xstep+(dy)*ystep])

    if (normy == 0)
        return 0.5 * (s0*vil2_I(-1,+0) + s1*vil2_I(+0,+0) + s2*vil2_I(+1,+0) + s3*vil2_I(+2,+0));

    double t0 = ((2-normy)*normy-1)*normy;
    double t1 = (3*normy-5)*normy*normy+2;
    double t2 = ((4-3*normy)*normy+1)*normy;
    double t3 = (normy-1)*normy*normy;

    // inefficiency: if normx is 0, then the s's were computed for nothing
    if (normx == 0)
        return 0.5 * (t0*vil2_I(+0,-1) + t1*vil2_I(+0,+0) + t2*vil2_I(+0,+1) + t3*vil2_I(+0,+2));

    double val = 0.25 *
                 ( (s0*vil2_I(-1,-1) + s1*vil2_I(+0,-1) + s2*vil2_I(+1,-1) + s3*vil2_I(+2,-1))*t0 +
                   (s0*vil2_I(-1,+0) + s1*vil2_I(+0,+0) + s2*vil2_I(+1,+0) + s3*vil2_I(+2,+0))*t1 +
                   (s0*vil2_I(-1,+1) + s1*vil2_I(+0,+1) + s2*vil2_I(+1,+1) + s3*vil2_I(+2,+1))*t2 +
                   (s0*vil2_I(-1,+2) + s1*vil2_I(+0,+2) + s2*vil2_I(+1,+2) + s3*vil2_I(+2,+2))*t3 );

#undef vil2_I

    return val;
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [1,nx-2]*[1,ny-2].
template<class T>
inline double vil2_bicub_interp_safe(double x, double y, const T* data,
                                     int nx, int ny,
                                     vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    if (x<1) return 0.0;
    if (y<1) return 0.0;
    if (x>nx-2) return 0.0;
    if (y>ny-2) return 0.0;
    return vil2_bicub_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2].
// \relates vil2_image_view
template<class T>
inline double vil2_bicub_interp_safe(const vil2_image_view<T> &view, 
                                     double x, double y, unsigned p=0)
{
    return vil2_bicub_interp_safe(x, y, &view(0,0,p),
                                  view.ni(), view.nj(),
                                  view.istep(), view.jstep());
}

//: Compute bicubic interpolation at (x,y), with minimal bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [1,nx-2]*[1,ny-2].
template<class T>
inline double vil2_bicub_interp(double x, double y, const T* data,
                                int nx, int ny,
                                vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    assert (x>=1);
    assert (y>=1);
    assert (x<=nx-2);
    assert (y<=ny-2);
    return vil2_bicub_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bicubic interpolation at (x,y), with minimal bound checks
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2].
// \relates vil2_image_view
template<class T>
inline double vil2_bicub_interp(const vil2_image_view<T> &view, 
                                double x, double y, unsigned p=0)
{
    return vil2_bicub_interp(x, y, &view(0,0,p),
                             view.ni(), view.nj(),
                             view.istep(), view.jstep());
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [1,nx-2]*[1,ny-2].
template<class T>
inline double vil2_bicub_interp_safe_extend(double x, double y, const T* data,
                                            int nx, int ny,
                                            vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    if (x<1) x= 0.0;
    if (y<1) y= 0.0;
    if (x>nx-2) x=nx-1.0;
    if (y>ny-2) y=ny-1.0;
    return vil2_bicub_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2].
// \relates vil2_image_view
template<class T>
inline double vil2_bicub_interp_safe_extend(const vil2_image_view<T> &view,
                                            double x, double y, unsigned p=0)
{
    return vil2_bicub_interp_safe_extend(x, y, &view(0,0,p),
                                         view.ni(), view.nj(),
                                         view.istep(), view.jstep());
}

#endif // vil2_bicub_interp_h_
