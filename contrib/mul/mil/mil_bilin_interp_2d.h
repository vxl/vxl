#ifndef mil_bilin_interp_2d_h_
#define mil_bilin_interp_2d_h_

//: \file
//  \brief Bilinear interpolation functions for 2D images
//  \author Tim Cootes

//: Compute bilinear interpolation at (x,y), no bound checks
//  Image is nx * ny array of T. x,y element is data[ystep*y+x]
//  No bound checks are done.


#include <vcl_cfloat.h>


template<class T>
inline double mil_bilin_interp_2d(double x, double y, const T* data, int ystep)
{
    int p1x,p1y;
    double normx,normy;
    p1x=int(x);
    normx = x-p1x;
    p1y=int(y);
    normy = y-p1y;

    const T* row1 = data + p1y*ystep + p1x;
    const T* row2 = row1 + ystep;

    double i1,i2;
    i1 = (double)row1[0]+
        (double)(row2[0]-row1[0])*normy;
    i2 = (double)row1[1]+
        (double)(row2[1]-row1[1])*normy;

    return i1+(i2-i1)*normx;
}

//: Compute bilinear interpolation at (x,y), no bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  No bound checks are done.
template<class T>
inline double mil_bilin_interp_2d(double x, double y, const T* data, int xstep, int ystep)
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
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside interpolatable image region, zero is returned.
template<class T>
inline double mil_safe_bilin_interp_2d(double x, double y, const T* data, int nx, int ny, int ystep)
{
    if (x<0) return 0.0;
    if (y<0) return 0.0;
    if (x>nx-1) return 0.0;
    if (y>ny-1) return 0.0;
    return mil_bilin_interp_2d(x,y,data,ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [0,nx)*[0,ny).
template<class T>
inline double mil_safe_bilin_interp_2d(double x, double y, const T* data,
                                       int nx, int ny, int xstep, int ystep)
{
    if (x<0) return 0.0;
    if (y<0) return 0.0;
    if (x>=nx-1) return 0.0;
    if (y>=ny-1) return 0.0;
    return mil_bilin_interp_2d(x,y,data,xstep,ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [0,nx)*[0,ny).
template<class T>
inline double mil_safe_extend_bilin_interp_2d(double x, double y, const T* data, int nx, int ny, int ystep)
{
    if (x<0) x= 0.0;
    if (y<0) y= 0.0;
    if (x>=nx-1) x=(double)nx-1.00000001;
    if (y>=ny-1) y=(double)ny-1.00000001;
    return mil_bilin_interp_2d(x,y,data,ystep);
}
//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [0,nx)*[0,ny).
template<class T>
inline double mil_safe_extend_bilin_interp_2d(double x, double y, const T* data, int nx, int ny, int xstep, int ystep)
{
    if (x<0) x= 0.0;
    if (y<0) y= 0.0;
    if (x>=nx-1) x=(double)nx-1.00000001;
    if (y>=ny-1) y=(double)ny-1.00000001;
    return mil_bilin_interp_2d(x,y,data,xstep,ystep);
}


#endif // mil_bilin_interp_2d_h_
