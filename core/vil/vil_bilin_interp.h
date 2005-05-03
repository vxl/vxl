// This is core/vil/vil_bilin_interp.h
#ifndef vil_bilin_interp_h_
#define vil_bilin_interp_h_
//:
// \file
// \brief Bilinear interpolation functions for 2D images
// \author Tim Cootes
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bicub file that would likely also benefit from
// the same change.

#include <vcl_cassert.h>
#include <vcl_cstddef.h>
#include <vil/vil_image_view.h>

//: Compute bilinear interpolation at (x,y), no bound checks. Requires 0<x<ni-2, 0<y<nj-2
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
inline double vil_bilin_interp_unsafe(double x, double y, const T* data,
                                      vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  int p1x=int(x);
  double normx = x-p1x;
  int p1y=int(y);
  double normy = y-p1y;

  const T* pix1 = data + p1y*ystep + p1x*xstep;

  double i1 = pix1[0    ]+(pix1[      ystep]-pix1[0    ])*normy;
  double i2 = pix1[xstep]+(pix1[xstep+ystep]-pix1[xstep])*normy;

  return i1+(i2-i1)*normx;
}


//: Compute bilinear interpolation at (x,y), no bound checks. Requires 0<x<ni-2, 0<y<nj-2
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
//  This is a version of vil_bilin_interp_unsafe with the same function
//  signature as vil_bilin_interp_safe.
template<class T>
inline double vil_bilin_interp_unsafe(double x, double y, const T* data,
                                      int /*nx*/, int /*ny*/,
                                      vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  return vil_bilin_interp_unsafe(x, y, data, xstep, ystep);
}

//: Compute bilinear interpolation at (x,y), no bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
inline double vil_bilin_interp_raw(double x, double y, const T* data,
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
  if (normx == 0) return pix1[0]+(pix1[ystep]-pix1[0])*normy;
  if (normy == 0) return pix1[0]+(pix1[xstep]-pix1[0])*normx;

  double i1 = pix1[0    ]+(pix1[      ystep]-pix1[0    ])*normy;
  double i2 = pix1[xstep]+(pix1[xstep+ystep]-pix1[xstep])*normy;

  return i1+(i2-i1)*normx;
}

//: Compute bilinear interpolation at (x,y), no bound checks.
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
//  This is a version of vil_bilin_interp_raw with the same function
//  signature as vil_bilin_interp_safe.
template<class T>
inline double vil_bilin_interp_raw(double x, double y, const T* data,
                                   int /*nx*/, int /*ny*/,
                                   vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  return vil_bilin_interp_raw(x, y, data, xstep, ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [0,nx-1]*[0,ny-1].
template<class T>
inline double vil_bilin_interp_safe(double x, double y, const T* data,
                                    int nx, int ny,
                                    vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  if (x<0) return 0.0;
  if (y<0) return 0.0;
  if (x>nx-1) return 0.0;
  if (y>ny-1) return 0.0;
  return vil_bilin_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [0,view.ni()-1]*[0,view.nj()-1].
// \relates vil_image_view
template<class T>
inline double vil_bilin_interp_safe(const vil_image_view<T> &view,
                                    double x, double y, unsigned p=0)
{
  return vil_bilin_interp_safe(x, y, &view(0,0,p),
                               view.ni(), view.nj(),
                               view.istep(), view.jstep());
}


//: Compute bilinear interpolation at (x,y), with minimal bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [0,nx-1]*[0,ny-1].
template<class T>
inline double vil_bilin_interp(double x, double y, const T* data,
                               int nx, int ny,
                               vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  assert (x>=0);
  assert (y>=0);
  assert (x<=nx-1);
  assert (y<=ny-1);
  return vil_bilin_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bilinear interpolation at (x,y), with minimal bound checks
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [0,view.ni()-1]*[0,view.nj()-1].
// \relates vil_image_view
template<class T>
inline double vil_bilin_interp(const vil_image_view<T> &view,
                               double x, double y, unsigned p=0)
{
  return vil_bilin_interp(x, y, &view(0,0,p),
                          view.ni(), view.nj(),
                          view.istep(), view.jstep());
}


//: Compute bilinear interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [0,nx-1]*[0,ny-1].
template<class T>
inline double vil_bilin_interp_safe_extend(double x, double y, const T* data,
                                           int nx, int ny,
                                           vcl_ptrdiff_t xstep, vcl_ptrdiff_t ystep)
{
  if (x<0) x= 0.0;
  if (y<0) y= 0.0;
  if (x>nx-1) x=nx-1.0;
  if (y>ny-1) y=ny-1.0;
  return vil_bilin_interp_raw(x,y,data,xstep,ystep);
}

//: Compute bilinear interpolation at (x,y), with bound checks
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [0,view.ni()-1]*[0,view.nj()-1].
// \relates vil_image_view
template<class T>
inline double vil_bilin_interp_safe_extend(const vil_image_view<T> &view,
                                           double x, double y, unsigned p=0)
{
  return vil_bilin_interp_safe_extend(x, y, &view(0,0,p),
                                      view.ni(), view.nj(),
                                      view.istep(), view.jstep());
}

#endif // vil_bilin_interp_h_
