#ifndef vil3d_resample_tricubic_hxx_
#define vil3d_resample_tricubic_hxx_
//:
// \file
// \brief Resample a 3D image by a tricubic method
// \author Gwenael Guillard

#include "vil3d_resample_tricubic.h"

#include <vil/vil_convert.h>
#include <vil3d/vil3d_tricub_interp.h>
#include <vil3d/vil3d_plane.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


inline bool vil3dresample_tricub_corner_in_image(double x0, double y0, double z0,
                                                 const vil3d_image_view_base& image)
{
  if (x0<1.0) return false;
  if (x0>image.ni()-3.0) return false;
  if (y0<1.0) return false;
  if (y0>image.nj()-3.0) return false;
  if (z0<1.0) return false;
  if (z0>image.nk()-3.0) return false;
  return true;
}


//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable region return zero or \a outval
template <class S, class T>
void vil3d_resample_tricubic(const vil3d_image_view<S>& src_image,
                             vil3d_image_view<T>& dest_image,
                             double x0, double y0, double z0,
                             double dx1, double dy1, double dz1,
                             double dx2, double dy2, double dz2,
                             double dx3, double dy3, double dz3,
                             int n1, int n2, int n3,
                             T outval/*=0*/)
{
  bool all_in_image =
    vil3dresample_tricub_corner_in_image(x0,
                                         y0,
                                         z0,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1,
                                         y0 + (n1-1)*dy1,
                                         z0 + (n1-1)*dz1,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n2-1)*dx2,
                                         y0 + (n2-1)*dy2,
                                         z0 + (n2-1)*dz2,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1 + (n2-1)*dx2,
                                         y0 + (n1-1)*dy1 + (n2-1)*dy2,
                                         z0 + (n1-1)*dz1 + (n2-1)*dz2,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n3-1)*dx3,
                                         y0 + (n3-1)*dy3,
                                         z0 + (n3-1)*dz3,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1 + (n3-1)*dx3,
                                         y0 + (n1-1)*dy1 + (n3-1)*dy3,
                                         z0 + (n1-1)*dz1 + (n3-1)*dz3,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n2-1)*dx2 + (n3-1)*dx3,
                                         y0 + (n2-1)*dy2 + (n3-1)*dy3,
                                         z0 + (n2-1)*dz2 + (n3-1)*dz3,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1 + (n2-1)*dx2 + (n3-1)*dx3,
                                         y0 + (n1-1)*dy1 + (n2-1)*dy2 + (n3-1)*dy3,
                                         z0 + (n1-1)*dz1 + (n2-1)*dz2 + (n3-1)*dz3,
                                         src_image);

  vil_convert_round_pixel<double, T> cast_and_possibly_round;

  const unsigned ni = src_image.ni();
  const unsigned nj = src_image.nj();
  const unsigned nk = src_image.nk();
  const unsigned np = src_image.nplanes();
  const std::ptrdiff_t istep = src_image.istep();
  const std::ptrdiff_t jstep = src_image.jstep();
  const std::ptrdiff_t kstep = src_image.kstep();
  const std::ptrdiff_t pstep = src_image.planestep();
  const S* plane0 = src_image.origin_ptr();

  dest_image.set_size(n1,n2,n3,np);
  const std::ptrdiff_t d_istep = dest_image.istep();
  const std::ptrdiff_t d_jstep = dest_image.jstep();
  const std::ptrdiff_t d_kstep = dest_image.kstep();
  const std::ptrdiff_t d_pstep = dest_image.planestep();
  T* d_plane0 = dest_image.origin_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
            cast_and_possibly_round( vil3d_tricub_interp_raw( x, y, z,
                                                              plane0,
                                                              istep, jstep, kstep),
                                     *dpt);
        }
      }
    }
    else
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
          {
            for (unsigned int p=0; p<np; ++p)
              cast_and_possibly_round( vil3d_tricub_interp_raw( x, y, z,
                                                                plane0+p*pstep,
                                                                istep, jstep, kstep),
                                       dpt[p*d_pstep]);
          }
        }
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
            cast_and_possibly_round( vil3d_tricub_interp_safe( x, y, z,
                                                               plane0,
                                                               ni, nj, nk,
                                                               istep, jstep, kstep,
                                                               outval),
                                     *dpt);
        }
      }
    }
    else
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
          {
            for (unsigned int p=0; p<np; ++p)
              cast_and_possibly_round( vil3d_tricub_interp_safe( x, y, z,
                                                                 plane0+p*pstep,
                                                                 ni, nj, nk,
                                                                 istep, jstep, kstep,
                                                                 outval),
                                       dpt[p*d_pstep]);
          }
        }
      }
    }
  }
}

//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the value of the nearest valid pixel.
template <class S, class T>
void vil3d_resample_tricubic_edge_extend(const vil3d_image_view<S>& src_image,
                                         vil3d_image_view<T>& dest_image,
                                         double x0, double y0, double z0,
                                         double dx1, double dy1, double dz1,
                                         double dx2, double dy2, double dz2,
                                         double dx3, double dy3, double dz3,
                                         int n1, int n2, int n3)
{
  bool all_in_image =
    vil3dresample_tricub_corner_in_image(x0,
                                         y0,
                                         z0,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1,
                                         y0 + (n1-1)*dy1,
                                         z0 + (n1-1)*dz1,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n2-1)*dx2,
                                         y0 + (n2-1)*dy2,
                                         z0 + (n2-1)*dz2,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1 + (n2-1)*dx2,
                                         y0 + (n1-1)*dy1 + (n2-1)*dy2,
                                         z0 + (n1-1)*dz1 + (n2-1)*dz2,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n3-1)*dx3,
                                         y0 + (n3-1)*dy3,
                                         z0 + (n3-1)*dz3,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1 + (n3-1)*dx3,
                                         y0 + (n1-1)*dy1 + (n3-1)*dy3,
                                         z0 + (n1-1)*dz1 + (n3-1)*dz3,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n2-1)*dx2 + (n3-1)*dx3,
                                         y0 + (n2-1)*dy2 + (n3-1)*dy3,
                                         z0 + (n2-1)*dz2 + (n3-1)*dz3,
                                         src_image) &&
    vil3dresample_tricub_corner_in_image(x0 + (n1-1)*dx1 + (n2-1)*dx2 + (n3-1)*dx3,
                                         y0 + (n1-1)*dy1 + (n2-1)*dy2 + (n3-1)*dy3,
                                         z0 + (n1-1)*dz1 + (n2-1)*dz2 + (n3-1)*dz3,
                                         src_image);

  vil_convert_round_pixel<double, T> cast_and_possibly_round;

  const unsigned ni = src_image.ni();
  const unsigned nj = src_image.nj();
  const unsigned nk = src_image.nk();
  const unsigned np = src_image.nplanes();
  const std::ptrdiff_t istep = src_image.istep();
  const std::ptrdiff_t jstep = src_image.jstep();
  const std::ptrdiff_t kstep = src_image.kstep();
  const std::ptrdiff_t pstep = src_image.planestep();
  const S* plane0 = src_image.origin_ptr();

  dest_image.set_size(n1,n2,n3,np);
  const std::ptrdiff_t d_istep = dest_image.istep();
  const std::ptrdiff_t d_jstep = dest_image.jstep();
  const std::ptrdiff_t d_kstep = dest_image.kstep();
  const std::ptrdiff_t d_pstep = dest_image.planestep();
  T* d_plane0 = dest_image.origin_ptr();

  if ( all_in_image )
  {
    if (np==1)
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
            cast_and_possibly_round( vil3d_tricub_interp_raw(x,y,z,
                                                             plane0,
                                                             istep,jstep,kstep),
                                     *dpt );
        }
      }
    }
    else
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
          {
            for (unsigned int p=0; p<np; ++p)
              cast_and_possibly_round( vil3d_tricub_interp_raw(x,y,z,
                                                               plane0+p*pstep,
                                                               istep,jstep,kstep),
                                       dpt[p*d_pstep] );
          }
        }
      }
    }
  }
  else
  {
    if (np==1)
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
            cast_and_possibly_round( vil3d_tricub_interp_safe_extend(x,y,z,
                                                                     plane0,
                                                                     ni,nj,nk,
                                                                     istep,jstep,kstep),
                                     *dpt );
        }
      }
    }
    else
    {
      double xk=x0, yk=y0, zk=z0;
      T *slice = d_plane0;
      for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
      {
        double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
        T *row = slice;
        for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
        {
          double x=xj, y=yj, z=zj;  // Start of j-th row
          T *dpt = row;
          for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
          {
            for (unsigned int p=0; p<np; ++p)
              cast_and_possibly_round( vil3d_tricub_interp_safe_extend(x,y,z,
                                                                       plane0+p*pstep,
                                                                       ni,nj,nk,
                                                                       istep,jstep,kstep),
                                       dpt[p*d_pstep] );
          }
        }
      }
    }
  }
}


//: Sample grid of points in one image and place in another, using tricubic interpolation and edge extension.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1].
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the value of the nearest valid pixel.
template <class S, class T>
void vil3d_resample_tricubic_edge_extend(const vil3d_image_view<S>& src_image,
                                         vil3d_image_view<T>& dest_image,
                                         int n1, int n2, int n3)
{
  double f= 0.9999999; // so sampler doesn't go off edge of image
  double x0=0;
  double y0=0;
  double z0=0;

  double dx1=f*(src_image.ni()-1)*1.0/(n1-1);
  double dy1=0;
  double dz1=0;

  double dx2=0;
  double dy2=f*(src_image.nj()-1)*1.0/(n2-1);
  double dz2=0;

  double dx3=0;
  double dy3=0;
  double dz3=f*(src_image.nk()-1)*1.0/(n3-1);

  vil3d_resample_tricubic_edge_extend(src_image, dest_image,
                                      x0, y0, z0,
                                      dx1, dy1, dz1, dx2, dy2, dz2, dx3, dy3, dz3,
                                      n1, n2, n3);
}


//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the trilinear interpolated value of the nearest valid pixels.
template <class S, class T>
void vil3d_resample_tricubic_edge_trilin_extend(const vil3d_image_view<S>& src_image,
                                                vil3d_image_view<T>& dest_image,
                                                double x0, double y0, double z0,
                                                double dx1, double dy1, double dz1,
                                                double dx2, double dy2, double dz2,
                                                double dx3, double dy3, double dz3,
                                                int n1, int n2, int n3)
{
  vil_convert_round_pixel<double, T> cast_and_possibly_round;

  const unsigned ni = src_image.ni();
  const unsigned nj = src_image.nj();
  const unsigned nk = src_image.nk();
  const unsigned np = src_image.nplanes();
  const std::ptrdiff_t istep = src_image.istep();
  const std::ptrdiff_t jstep = src_image.jstep();
  const std::ptrdiff_t kstep = src_image.kstep();
  const std::ptrdiff_t pstep = src_image.planestep();
  const S* plane0 = src_image.origin_ptr();

  dest_image.set_size(n1,n2,n3,np);
  const std::ptrdiff_t d_istep = dest_image.istep();
  const std::ptrdiff_t d_jstep = dest_image.jstep();
  const std::ptrdiff_t d_kstep = dest_image.kstep();
  const std::ptrdiff_t d_pstep = dest_image.planestep();
  T* d_plane0 = dest_image.origin_ptr();
  // Use safe interpolation with edge-extension
  if (np==1)
  {
    double xk=x0, yk=y0, zk=z0;
    T *slice = d_plane0;
    for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
    {
      double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
      T *row = slice;
      for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
      {
        double x=xj, y=yj, z=zj;  // Start of j-th row
        T *dpt = row;
        for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
          cast_and_possibly_round( vil3d_tricub_interp_safe_trilinear_extend(x,y,z,
                                                                             plane0,
                                                                             ni,nj,nk,
                                                                             istep,jstep,kstep),
                                   *dpt );
      }
    }
  }
  else
  {
    double xk=x0, yk=y0, zk=z0;
    T *slice = d_plane0;
    for (int k=0; k<n3; ++k, xk+=dx3, yk+=dy3, zk+=dz3, slice+=d_kstep)
    {
      double xj=xk, yj=yk, zj=zk;  // Start of k-th slice
      T *row = slice;
      for (int j=0; j<n2; ++j, xj+=dx2, yj+=dy2, zj+=dz2, row+=d_jstep)
      {
        double x=xj, y=yj, z=zj;  // Start of j-th row
        T *dpt = row;
        for (int i=0; i<n1; ++i, x+=dx1, y+=dy1, z+=dz1, dpt+=d_istep)
        {
          for (unsigned int p=0; p<np; ++p)
            cast_and_possibly_round( vil3d_tricub_interp_safe_trilinear_extend(x,y,z,
                                                                               plane0+p*pstep,
                                                                               ni,nj,nk,
                                                                               istep,jstep,kstep),
                                     dpt[p*d_pstep] );
        }
      }
    }
  }
}

//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3),
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside interpolatable return the trilinear interpolated value of the nearest valid pixels.
template <class S, class T>
void vil3d_resample_tricubic_edge_trilin_extend(const vil3d_image_view<S>& src_image,
                                                vil3d_image_view<T>& dest_image,
                                                int n1, int n2, int n3)
{
  double f= 0.9999999; // so sampler doesn't go off edge of image
  double x0=0;
  double y0=0;
  double z0=0;

  double dx1=f*(src_image.ni()-1)*1.0/(n1-1);
  double dy1=0;
  double dz1=0;

  double dx2=0;
  double dy2=f*(src_image.nj()-1)*1.0/(n2-1);
  double dz2=0;

  double dx3=0;
  double dy3=0;
  double dz3=f*(src_image.nk()-1)*1.0/(n3-1);

  vil3d_resample_tricubic_edge_trilin_extend(src_image, dest_image,
                                             x0, y0, z0,
                                             dx1, dy1, dz1, dx2, dy2, dz2, dx3, dy3, dz3,
                                             n1, n2, n3);
}


//: Resample image to a specified dimensions (n1 * n2 * n3)
template <class S, class T>
void vil3d_resample_tricubic(const vil3d_image_view<S>& src_image,
                             vil3d_image_view<T>& dest_image,
                             int n1, int n2, int n3)
{
  double f= 0.9999999; // so sampler doesn't go off edge of image
  double x0=0;
  double y0=0;
  double z0=0;

  double dx1=f*(src_image.ni()-1)*1.0/(n1-1);
  double dy1=0;
  double dz1=0;

  double dx2=0;
  double dy2=f*(src_image.nj()-1)*1.0/(n2-1);
  double dz2=0;

  double dx3=0;
  double dy3=0;
  double dz3=f*(src_image.nk()-1)*1.0/(n3-1);

  vil3d_resample_tricubic(src_image, dest_image,
                          x0, y0, z0,
                          dx1, dy1, dz1,
                          dx2, dy2, dz2,
                          dx3, dy3, dz3,
                          n1, n2, n3);
}


#define VIL3D_RESAMPLE_TRICUBIC_INSTANTIATE( S, T ) \
template void vil3d_resample_tricubic(const vil3d_image_view< S >& src_image, \
                                      vil3d_image_view< T >& dest_image, \
                                      double x0, double y0, double z0, \
                                      double dx1, double dy1, double dz1, \
                                      double dx2, double dy2, double dz2, \
                                      double dx3, double dy3, double dz3, \
                                      int n1, int n2, int n3, \
                                      T outval); \
template void vil3d_resample_tricubic_edge_extend(const vil3d_image_view< S >& src_image, \
                                                  vil3d_image_view< T >& dest_image, \
                                                  double x0, double y0, double z0, \
                                                  double dx1, double dy1, double dz1, \
                                                  double dx2, double dy2, double dz2, \
                                                  double dx3, double dy3, double dz3, \
                                                  int n1, int n2, int n3); \
template void vil3d_resample_tricubic_edge_extend(const vil3d_image_view< S >& src_image, \
                                                  vil3d_image_view< T >& dest_image, \
                                                  int n1, int n2, int n3); \
template void vil3d_resample_tricubic_edge_trilin_extend(const vil3d_image_view< S >& src_image, \
                                                         vil3d_image_view< T >& dest_image, \
                                                         double x0, double y0, double z0, \
                                                         double dx1, double dy1, double dz1, \
                                                         double dx2, double dy2, double dz2, \
                                                         double dx3, double dy3, double dz3, \
                                                         int n1, int n2, int n3); \
template void vil3d_resample_tricubic_edge_trilin_extend(const vil3d_image_view< S >& src_image, \
                                                         vil3d_image_view< T >& dest_image, \
                                                         int n1, int n2, int n3); \
template void vil3d_resample_tricubic(const vil3d_image_view< S >& src_image, \
                                      vil3d_image_view< T >& dest_image, \
                                      int n1, int n2, int n3)
#if 0
template void vil3d_resample_trilinear(const vil3d_image_view< T >& src_image, \
                                       vil3d_image_view< T >& dst_image, \
                                       const double dx, \
                                       const double dy, \
                                       const double dz); \
template void vil3d_resample_trilinear_scale_2(const vil3d_image_view< T >& src_image, \
                                               vil3d_image_view< T >& dst_image)
#endif

#endif // vil3d_resample_tricubic_hxx_
