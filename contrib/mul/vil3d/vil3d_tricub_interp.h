#ifndef vil3d_tricub_interp_h_
#define vil3d_tricub_interp_h_
//:
// \file
// \brief Tricubic interpolation functions for 3D images
//
#include <iostream>
#include <cstddef>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_trilin_interp.h>

//: Compute tricubic interpolation at (x,y,z), no bound checks
//  Image is nx * ny * nz array of T. x,y,z element is data[z*zstep+ystep*y+x*xstep]
//  No bound checks are done.
template<class T>
double vil3d_tricub_interp_raw(double x, double y, double z, const T* data,
                               std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep);

//: Compute tricubic interpolation at (x,y,z), with bound checks
//  Image is nx * ny * nz array of T. x,y,z element is data[z*zstep+ystep*y+x*xstep]
//  If (x,y,z) is outside interpolatable image region, returns zero or \a outval
//  The safe interpolatable region is [1,nx-3]*[1,ny-3]*[1,nz-3].
template<class T>
inline double vil3d_tricub_interp_safe(double x, double y, double z, const T* data,
                                       int nx, int ny, int nz,
                                       std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep,
                                       T outval=0)
{
    if (x<1) return static_cast<double>(outval);
    if (y<1) return static_cast<double>(outval);
    if (z<1) return static_cast<double>(outval);
    if (x>nx-3) return static_cast<double>(outval);
    if (y>ny-3) return static_cast<double>(outval);
    if (z>nz-3) return static_cast<double>(outval);
    return vil3d_tricub_interp_raw(x,y,z,data,xstep,ystep,zstep);
}

//: Compute tricubic interpolation at (x,y,z), with minimal bound checks
//  Image is nx * ny * nz array of Ts. x,y,z element is data[ystep*y+xstep*x]
//  If (x,y,z) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [1,nx-3]*[1,ny-3]*[1,nz-3].
template<class T>
inline double vil3d_tricub_interp_assert(double x, double y, double z, const T* data,
                                         int nx, int ny, int nz,
                                         std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
    assert (x>=1);
    assert (y>=1);
    assert (z>=1);
    assert (x<=nx-3);
    assert (y<=ny-3);
    assert (z<=nz-3);
    return vil3d_tricub_interp_raw(x,y,z,data,xstep,ystep,zstep);
}

//: Compute tricubic interpolation at (x,y,z), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y,z) is outside safe interpolatable image region, nearest pixel value is returned.
//  The safe interpolatable region is [1,nx-3]*[1,ny-3]*[1,nz-3].
template<class T>
inline double vil3d_tricub_interp_safe_extend(double x, double y, double z,
                                              const T* data,
                                              int nx, int ny, int nz,
                                              std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  if (x<0.9999999) x= 0.0;
  else if (x>nx-3.0000001 && x<nx-1.9999999) x=nx-2.0;
  else if (x>nx-2.0000001) x=nx-1.0;

  if (y<0.9999999) y= 0.0;
  else if (y>ny-3.0000001 && y<ny-1.9999999) y=ny-2.0;
  else if (y>ny-2.0000001) y=ny-1.0;

  if (z<0.9999999) z= 0.0;
  else if (z>nz-3.0000001 && z<nz-1.9999999) z=nz-2.0;
  else if (z>nz-2.0000001) z=nz-1.0;

  return vil3d_tricub_interp_raw(x,y,z,data,xstep,ystep,zstep);
}

//: Compute tricubic interpolation at (x,y,z), with bound checks
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y,z) is outside safe interpolatable image region, trilinear interpolated value of the nearest valid pixels is returned.
//  The safe interpolatable region is [1,nx-3]*[1,ny-3]*[1,nz-3].
template<class T>
double vil3d_tricub_interp_safe_trilinear_extend(double x, double y, double z,
                                                 const T* data,
                                                 int nx, int ny, int nz,
                                                 std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep);


// //: Compute tricubic interpolation at (x,y,z), with bound checks
// //  If (x,y,z) is outside safe interpolatable image region, nearest pixel value is returned.
// //  The safe interpolatable region is [1,view.ni()-2]*[1,view.nj()-2]*[1,view.nk()-2].
// // \relatesalso vil3d_image_view
// template<class T>
// inline double vil3d_tricub_interp_safe_extend(const vil3d_image_view<T> &view,
//                                               double x, double y, double z, unsigned p=0)
// {
//   return vil3d_tricub_interp_safe_extend(x, y, z, &view(0,0,p),
//                                          view.ni(), view.nj(), view.nk(),
//                                          view.istep(), view.jstep(), view.kstep() );
// }

#endif // vil3d_tricub_interp_h_
