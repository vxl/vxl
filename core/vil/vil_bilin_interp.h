// This is mul/vil2/vil2_bilin_interp.h
#ifndef vil2_bilin_interp_h_
#define vil2_bilin_interp_h_
//: \file
//  \brief Bilinear interpolation functions for 2D images
//  \author Tim Cootes

#include <vcl_cassert.h>

//: Compute bilinear interpolation at (x,y), no bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  No bound checks are done.
template<class T>
inline double vil2_bilin_interp(double x, double y, const T* data, int xstep, int ystep)
{
    int p1x,p1y;
    double normx,normy;
    p1x=int(x);
    normx = x-p1x;
    p1y=int(y);
    normy = y-p1y;

    const T* row1 = data + p1y*ystep + p1x * xstep;
    const T* row2 = row1 + ystep;

    double i1,i2;
    i1 = (double)row1[0]+
        (double)(row2[0]-row1[0])*normy;
    i2 = (double)row1[xstep]+
        (double)(row2[xstep]-row1[xstep])*normy;

    return i1+(i2-i1)*normx;
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [0,nx)*[0,ny).
template<class T>
inline double vil2_safe_bilin_interp(double x, double y, const T* data,
                                     int nx, int ny, int xstep, int ystep)
{
    if (x<0) return 0.0;
    if (y<0) return 0.0;
    if (x>=nx-1) return 0.0;
    if (y>=ny-1) return 0.0;
    return vil2_bilin_interp(x,y,data,xstep,ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [0,nx)*[0,ny).
template<class T>
inline double vil2_assert_bilin_interp(double x, double y, const T* data,
                                       int nx, int ny, int xstep, int ystep)
{
    assert (x>=0);
    assert (y>=0);
    assert (x<nx-1);
    assert (y<ny-1);
    return vil2_bilin_interp(x,y,data,xstep,ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [0,nx)*[0,ny).
template<class T>
inline double vil2_safe_extend_bilin_interp(double x, double y, const T* data, int nx, int ny, int xstep, int ystep)
{
    if (x<0) x= 0.0;
    if (y<0) y= 0.0;
    if (x>=nx-1) x=(double)nx-1.00000001;
    if (y>=ny-1) y=(double)ny-1.00000001;
    return vil2_bilin_interp(x,y,data,xstep,ystep);
}

#endif // vil2_bilin_interp_h_
