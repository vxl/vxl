// This is core/vil/vil_bicub_interp.h
#ifndef vil_bicub_interp_h_
#define vil_bicub_interp_h_
//:
// \file
// \brief Bicubic interpolation functions for 2D images
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin file that would likely also benefit from
// the same change.

#include <vcl_cassert.h>
#include <vcl_cstddef.h>
#include <vil/vil_image_view.h>

//: Compute bicubic interpolation at (x,y), no bound checks. Requires 1<x<ni-3, 1<y<nj-3
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
double vil_bicub_interp_unsafe(double x, double y, const T* data,
                               vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep);

//: Compute bicubic interpolation at (x,y), no bound checks. Requires 1<x<ni-3, 1<y<nj-3
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
//  This is a version of vil_bicub_interp_unsafe with the same function
//  signature as vil_bicub_interp_safe.
template<class T>
inline double vil_bicub_interp_unsafe(double x, double y, const T* data,
                                      int /*nx*/, int /*ny*/,
                                      vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  return vil_bicub_interp_unsafe(x, y, data, xstep, ystep);
}



//: Compute bicubic interpolation at (x,y), no bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
double vil_bicub_interp_raw(double x, double y, const T* data,
                            vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep);

                            
//: Compute bicubic interpolation at (x,y), no bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
//  This is a version of vil_bicub_interp_raw with the same function
//  signature as vil_bicub_interp_safe.
template<class T>
inline double vil_bicub_interp_raw(double x, double y, const T* data,
                                   int /*nx*/, int /*ny*/,
                                   vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  return vil_bicub_interp_raw(x, y, data, xstep, ystep);
}
                            
//: Compute bicubic interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [1,nx-2]*[1,ny-2].
template<class T>
inline double vil_bicub_interp_safe(double x, double y, const T* data,
                                    int nx, int ny,
                                    vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    if (x<1) return 0.0;
    if (y<1) return 0.0;
    if (x>nx-2) return 0.0;
    if (y>ny-2) return 0.0;
    return vil_bicub_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2].
// \relates vil_image_view
template<class T>
inline double vil_bicub_interp_safe(const vil_image_view<T> &view,
                                    double x, double y, unsigned p=0)
{
    return vil_bicub_interp_safe(x, y, &view(0,0,p),
                                 view.ni(), view.nj(),
                                 view.istep(), view.jstep());
}

//: Compute bicubic interpolation at (x,y), with minimal bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [1,nx-2]*[1,ny-2].
template<class T>
inline double vil_bicub_interp(double x, double y, const T* data,
                               int nx, int ny,
                               vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    assert (x>=1);
    assert (y>=1);
    assert (x<=nx-2);
    assert (y<=ny-2);
    return vil_bicub_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bicubic interpolation at (x,y), with minimal bound checks
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2].
// \relates vil_image_view
template<class T>
inline double vil_bicub_interp(const vil_image_view<T> &view,
                               double x, double y, unsigned p=0)
{
    return vil_bicub_interp(x, y, &view(0,0,p),
                            view.ni(), view.nj(),
                            view.istep(), view.jstep());
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [1,nx-2]*[1,ny-2].
template<class T>
inline double vil_bicub_interp_safe_extend(double x, double y, const T* data,
                                           int nx, int ny,
                                           vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
    if (x<1) x= 0.0;
    if (y<1) y= 0.0;
    if (x>nx-2) x=nx-1.0;
    if (y>ny-2) y=ny-1.0;
    return vil_bicub_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bicubic interpolation at (x,y), with bound checks
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2].
// \relates vil_image_view
template<class T>
inline double vil_bicub_interp_safe_extend(const vil_image_view<T> &view,
                                           double x, double y, unsigned p=0)
{
    return vil_bicub_interp_safe_extend(x, y, &view(0,0,p),
                                        view.ni(), view.nj(),
                                        view.istep(), view.jstep());
}

#endif // vil_bicub_interp_h_
