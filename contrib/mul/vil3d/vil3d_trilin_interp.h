// This is mul/vil3d/vil3d_trilin_interp.h
#ifndef vil3d_trilin_interp_h_
#define vil3d_trilin_interp_h_
//:
// \file
// \brief Trilinear interpolation functions for 3D images
// \author Tim Cootes

#include <iostream>
#include <cstddef>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_na.h>
#include <vil3d/vil3d_image_view.h>

//: Compute trilinear interpolation at (x,y,z), no bound checks
//  Image is nx * ny * nz array of T. x,y,z element is data[z*zstep+ystep*y+x*xstep]
//  No bound checks are done.
template<class T>
inline double vil3d_trilin_interp_raw(double x, double y, double z,
                                      const T* data, std::ptrdiff_t xstep,
                                      std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  int p1x,p1y,p1z;
  double normx,normy,normz;
  p1x=int(x);
  normx = x-p1x;
  p1y=int(y);
  normy = y-p1y;
  p1z=int(z);
  normz = z-p1z;

  const T* row11 = data + p1z*zstep+p1y*ystep + p1x*xstep;
  const T* row21 = row11 + ystep;
  const T* row12 = row11 + zstep;
  const T* row22 = row21 + zstep;

  // Bilinear interpolation in plane z=p1z
  double i11 = (double)row11[0]+(double)(row21[0]-row11[0])*normy;
  double i21 = (double)row11[xstep]+(double)(row21[xstep]-row11[xstep])*normy;
  double iz1 = i11+(i21-i11)*normx;

  // Bilinear interpolation in plane z=p1z+1
  double i12 = (double)row12[0]+(double)(row22[0]-row12[0])*normy;
  double i22 = (double)row12[xstep]+(double)(row22[xstep]-row12[xstep])*normy;
  double iz2 = i12+(i22-i12)*normx;

  return iz1+(iz2-iz1)*normz;
}


//: Compute trilinear interpolation at (x,y,z), with bound checks
//  Image is nx * ny * nz array of T. x,y,z element is data[z*zstep+ystep*y+x*xstep]
//  If (x,y,z) is outside interpolatable image region, returns zero or \a outval
template<class T>
inline double vil3d_trilin_interp_safe(double x, double y, double z, const T* data,
                                       unsigned nx, unsigned ny, unsigned nz,
                                       std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep,
                                       double outval=0)
{
  if (x<0) return static_cast<double>(outval);
  if (y<0) return static_cast<double>(outval);
  if (z<0) return static_cast<double>(outval);
  if (x>=nx-1) return static_cast<double>(outval);
  if (y>=ny-1) return static_cast<double>(outval);
  if (z>=nz-1) return static_cast<double>(outval);
  return vil3d_trilin_interp_raw(x,y,z,data,xstep,ystep,zstep);
}

//: Compute trilinear interpolation at (x,y,z,p), with bound checks
//  Image is nx * ny * nz array of T. x,y,z element is data[z*zstep+ystep*y+x*xstep]
//  If (x,y,z) is outside interpolatable image region, returns zero or \a outval
template<class T>
inline double vil3d_trilin_interp_safe(const vil3d_image_view<T>& image,
                                       double x, double y, double z,
                                       unsigned p=0,
                                       T outval=0)
{
  return vil3d_trilin_interp_safe(x,y,z,&image(0,0,0,p),
                                  image.ni(),image.nj(),image.nk(),
                                  image.istep(),image.jstep(),image.kstep(),
                                  outval);
}

//: Compute trilinear interpolation at (x,y), with bound checks
//  Image is nx * ny * nz array of Ts. x,y element is data[nx*y+x]
//  If (x,y) is outside interpolatable image region and NDEBUG is not defined
//  the code will fail an ASSERT.
//  The safe interpolatable region is [0,nx)*[0,ny)*[0,nz].
template<class T>
inline double vil3d_trilin_interp_assert(double x, double y, double z, const T* data,
                                         unsigned nx, unsigned ny, unsigned nz,
                                         std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  assert(x>=0);
  assert(y>=0);
  assert(z>=0);
  assert(x<nx-1);
  assert(y<ny-1);
  assert(z<nz-1);
  return vil3d_trilin_interp_raw(x,y,z,data,xstep,ystep,zstep);
}

//: Compute trilinear interpolation at (x,y), with bounds checks.
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y,z) is outside safe interpolatable image region, nearest pixel value is returned.
template<class T>
inline double vil3d_trilin_interp_safe_extend(double x, double y, double z, const T* data,
                                              unsigned nx, unsigned ny, unsigned nz,
                                              std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  if (x<0) x= 0.0;
  if (y<0) y= 0.0;
  if (z<0) z= 0.0;
  if (x>=nx-1) x=(double)nx-1.00000001;
  if (y>=ny-1) y=(double)ny-1.00000001;
  if (z>=nz-1) z=(double)nz-1.00000001;
  return vil3d_trilin_interp_raw(x,y,z,data,xstep,ystep,zstep);
}

//: Compute trilinear interpolation at (x,y), with bounds checks.
//  Image is nx * ny array of Ts. x,y element is data[nx*y+x]
//  If (x,y,z) is outside safe interpolatable image region, NA is returned.
template<class T>
inline double vil3d_trilin_interp_safe_edgena(double x, double y, double z, const T* data,
                                              unsigned nx, unsigned ny, unsigned nz,
                                              std::ptrdiff_t xstep, std::ptrdiff_t ystep, std::ptrdiff_t zstep)
{
  if (x<0 || y<0 || z<0 ||
    x>=nx-1 || y>=ny-1 || z>=nz-1) return vil_na(double());
  return vil3d_trilin_interp_raw(x,y,z,data,xstep,ystep,zstep);
}
//: Compute trilinear interpolation at (x,y), using the nearest valid value if out of bounds
//  If (x,y,z) is outside safe interpolatable image region, nearest pixel value is returned.
template<class T>
inline double vil3d_trilin_interp_safe_extend(const vil3d_image_view<T>& image,
                                              double x, double y, double z,
                                              unsigned p=0)
{
  return vil3d_trilin_interp_safe_extend(x, y, z, &image(0,0,0,p),
    image.ni(), image.nj(), image.nk(),
    image.istep(), image.jstep(), image.kstep());
}


#endif // vil3d_trilin_interp_h_
