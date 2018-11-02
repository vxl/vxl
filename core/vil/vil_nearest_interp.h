// This is core/vil/vil_nearest_interp.h
#ifndef vil_nearest_interp_h_
#define vil_nearest_interp_h_
//:
// \file
// \brief nearest neighbour interpolation functions for 2D images
// \author Ian Scott
//
// The vil_nearest_neighbour_interp source files were derived from the corresponding
// vil_bilin_interp files, thus the bilin/nearest_neighbour/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bicub and bilin files that would likely also benefit from
// the same change.

#include <cstddef>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_fwd.h>

//: Compute nearest neighbour interpolation at (x,y), no bound checks. Requires -0.5<x<ni-0.5, -0.5<y<nj-0.5
//  Image is nx * ny array of Ts. x,y element is data[xstep*x+ystep*y]
//  No bound checks are done.
template<class T>
inline T vil_nearest_interp_unsafe(double x, double y, const T* data,
                                   int /*nx*/, int /*ny*/,
                                   std::ptrdiff_t xstep, std::ptrdiff_t ystep)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  return *(data + ix*xstep + iy*ystep);
}


//: Compute nearest neighbour interpolation at (x,y), no bound checks. Requires -0.5<=x<ni-0.5, -0.5<=y<nj-0.5
//  No bound checks are done.
//  \relatesalso vil_image_view
template<class T>
inline T vil_nearest_interp_unsafe(const vil_image_view<T>& view, double x, double y, unsigned p=0)
{
  return vil_nearest_interp_unsafe(x, y, &view(0,0,p), 0, 0, view.istep(), view.jstep());
}

//: Compute nearest neighbour interpolation at (x,y), with bound checks
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[0,view.nj()-0.5).
template<class T>
inline T vil_nearest_interp_safe(double x, double y, const T* data,
                                 int nx, int ny,
                                 std::ptrdiff_t xstep, std::ptrdiff_t ystep)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  if (ix >= 0 && ix < nx && iy >= 0 && iy < ny)
    return *(data + ix*xstep + iy*ystep);
  else
    return 0;
}


//: Compute nearest neighbour interpolation at (x,y), with bound checks
//  If (x,y) is outside interpolatable image region, zero is returned.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[0,view.nj()-0.5).
// \relatesalso vil_image_view
template<class T>
inline T vil_nearest_interp_safe(
  const vil_image_view<T> &view, double x, double y, unsigned p=0)
{
  return vil_nearest_interp_safe(x, y, &view(0,0,p), view.ni(), view.nj(),
                                 view.istep(), view.jstep());
}


//: Compute nearest neighbour interpolation at (x,y), with minimal bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[0.5,view.nj()-0.5).
template<class T>
inline T vil_nearest_interp(double x, double y, const T* data,
                            int nx, int ny,
                            std::ptrdiff_t xstep, std::ptrdiff_t ystep)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  assert (ix>=0);
  assert (iy>=0);
  assert (ix<nx);
  assert (iy<ny);
  return *(data + ix*xstep + iy*ystep);
}


//: Compute nearest neighbour interpolation at (x,y), with minimal bound checks
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[0.5,view.nj()-0.5).
// \relatesalso vil_image_view
template<class T>
inline T vil_nearest_interp(
  const vil_image_view<T> &view, double x, double y, unsigned p=0)
{
  return vil_nearest_interp(x, y, &view(0,0,p), view.ni(), view.nj(),
                            view.istep(), view.jstep());
}


//: Compute nearest neighbour interpolation at (x,y), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[ystep*y+xstep*x]
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[-0.5,view.nj()-0.5).
template<class T>
inline T vil_nearest_interp_safe_extend(double x, double y, const T* data,
                                        int nx, int ny,
                                        std::ptrdiff_t xstep, std::ptrdiff_t ystep)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  if (ix<0)
    ix= 0;
  else
    if (ix>=nx) ix=nx;

  if (iy<0)
    iy= 0;
  else
    if (iy>=ny) iy=ny;

  return *(data + ix*xstep + iy*ystep);
}


//: Compute nearest neighbour interpolation at (x,y), with bound checks
//  If (x,y) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [-0.5,view.ni()-0.5)*[-0.5,view.nj()-0.5).
// \relatesalso vil_image_view
template<class T>
inline T vil_nearest_interp_safe_extend(
  const vil_image_view<T> &view, double x, double y, unsigned p=0)
{
  int ix = int(x + 0.5);
  int iy = int(y + 0.5);
  if (ix<0)
    ix= 0.0;
  else
    if (ix>=(int)view.ni()) ix=view.ni()-1;

  if (iy<0)
    iy= 0.0;
  else
    if (iy>=(int)view.nj()) iy=view.nj()-1;

  return view(ix, iy, p);
}

#endif // vil_nearest_interp_h_
