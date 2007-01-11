// This is mul/vil3d/vil3d_resample_trilinear.txx
#ifndef vil3d_resample_trilinear_txx_
#define vil3d_resample_trilinear_txx_


//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza, Ian Scott


#include <vil3d/vil3d_resample_trilinear.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vil/vil_convert.h>
#include <vil3d/vil3d_plane.h>
#include <vcl_cassert.h>


inline bool vil3d_grid_corner_in_image(double x0, double y0, double z0,
                                       const vil3d_image_view_base& image)
{
  if (x0<0.0) return false;
  if (x0>image.ni()-1.0) return false;
  if (y0<0.0) return false;
  if (y0>image.nj()-1.0) return false;
  if (z0<0.0) return false;
  if (z0>image.nk()-1.0) return false;
  return true;
}


//  Sample grid of points in one image and place in another, using trilinear interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2+k.dx3, y0+i.dy1+j.dy2+k.dy3, z0+i.dz1+j.dz2+k.dz3), 
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1]
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//  Points outside image return zero.
template <class S, class T>
void vil3d_resample_trilinear(const vil3d_image_view<S>& src_image,
                              vil3d_image_view<T>& dest_image,
                              double x0, double y0, double z0,
                              double dx1, double dy1, double dz1,
                              double dx2, double dy2, double dz2,
                              double dx3, double dy3, double dz3,
                              int n1, int n2, int n3)
{
  bool all_in_image = 
    vil3d_grid_corner_in_image(x0,
                               y0,
                               z0,
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n1-1)*dx1,            
                               y0 + (n1-1)*dy1,
                               z0 + (n1-1)*dz1, 
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n2-1)*dx2,            
                               y0 + (n2-1)*dy2,
                               z0 + (n2-1)*dz2, 
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n1-1)*dx1 + (n2-1)*dx2, 
                               y0 + (n1-1)*dy1 + (n2-1)*dy2,
                               z0 + (n1-1)*dz1 + (n2-1)*dz2, 
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n3-1)*dx3,            
                               y0 + (n3-1)*dy3,
                               z0 + (n3-1)*dz3, 
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n1-1)*dx1 + (n3-1)*dx3,            
                               y0 + (n1-1)*dy1 + (n3-1)*dy3,
                               z0 + (n1-1)*dz1 + (n3-1)*dz3, 
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n2-1)*dx2 + (n3-1)*dx3,            
                               y0 + (n2-1)*dy2 + (n3-1)*dy3,
                               z0 + (n2-1)*dz2 + (n3-1)*dz3, 
                               src_image) &&
    vil3d_grid_corner_in_image(x0 + (n1-1)*dx1 + (n2-1)*dx2 + (n3-1)*dx3,            
                               y0 + (n1-1)*dy1 + (n2-1)*dy2 + (n3-1)*dy3,
                               z0 + (n1-1)*dz1 + (n2-1)*dz2 + (n3-1)*dz3, 
                               src_image);
                     
#ifndef NDEBUG
  // corners
  vcl_cout<<"src_image= "<<src_image<<vcl_endl
          <<"x0="<<x0<<vcl_endl
          <<"y0="<<y0<<vcl_endl
          <<"z0="<<z0<<vcl_endl
          <<"x0 + (n1-1)*dx1 + (n2-1)*dx2 + (n3-1)*dx3="
          <<x0 + (n1-1)*dx1 + (n2-1)*dx2 + (n3-1)*dx3<<vcl_endl
          <<"y0 + (n1-1)*dy1 + (n2-1)*dy2 + (n3-1)*dy3="
          <<y0 + (n1-1)*dy1 + (n2-1)*dy2 + (n3-1)*dy3<<vcl_endl
          <<"z0 + (n1-1)*dz1 + (n2-1)*dz2 + (n3-1)*dz3="
          <<z0 + (n1-1)*dz1 + (n2-1)*dz2 + (n3-1)*dz3<<vcl_endl;         
#endif

  const unsigned ni = src_image.ni();
  const unsigned nj = src_image.nj();
  const unsigned nk = src_image.nk();
  const unsigned np = src_image.nplanes();
  const vcl_ptrdiff_t istep = src_image.istep();
  const vcl_ptrdiff_t jstep = src_image.jstep();
  const vcl_ptrdiff_t kstep = src_image.kstep();
  const vcl_ptrdiff_t pstep = src_image.planestep();
  const S* plane0 = src_image.origin_ptr();

  dest_image.set_size(n1,n2,n3,np);
  const vcl_ptrdiff_t d_istep = dest_image.istep();
  const vcl_ptrdiff_t d_jstep = dest_image.jstep();
  const vcl_ptrdiff_t d_kstep = dest_image.kstep();
  const vcl_ptrdiff_t d_pstep = dest_image.planestep();
  T* d_plane0 = dest_image.origin_ptr();

  double x1=x0;
  double y1=y0;
  double z1=z0;

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
            *dpt = (T) vil3d_trilin_interp_raw(x, y, z, plane0, 
                                               istep, jstep, kstep);
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
              dpt[p*d_pstep] = (T) vil3d_trilin_interp_raw(x, y, z, 
                                   plane0+p*pstep, istep, jstep, kstep);
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
            *dpt = (T) vil3d_trilin_interp_safe(x, y, z, 
              plane0, ni, nj, nk, istep, jstep, kstep);
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
              dpt[p*d_pstep] = (T) vil3d_trilin_interp_safe(x, y, z, 
                plane0+p*pstep, ni, nj, nk, istep, jstep, kstep);
          }
        }
      }
    }
  }
}


//  Resample image to a specified dimensions (n1 * n2 * n3)
template <class S, class T>
void vil3d_resample_trilinear(const vil3d_image_view<S>& src_image,
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

  vil3d_resample_trilinear(src_image, dest_image, x0, y0, z0, dx1, dy1, dz1,
                           dx2, dy2, dz2, dx3, dy3, dz3, n1, n2, n3);
}


// Resample a 3D image by a different factor in each dimension.
// Note: The upper image boundaries are extended.
template <class T>
void vil3d_resample_trilinear(const vil3d_image_view<T>& src_image,
                              vil3d_image_view<T>& dst_image,
                              const double dx,
                              const double dy,
                              const double dz)
{
  assert (dx > 0.0 && dy > 0.0 && dz > 0.0);

  // Assume planes are the same for both images
  const unsigned np = src_image.nplanes();

  const unsigned sni = src_image.ni();
  const unsigned snj = src_image.nj();
  const unsigned snk = src_image.nk();
  const vcl_ptrdiff_t s_istep = src_image.istep();
  const vcl_ptrdiff_t s_jstep = src_image.jstep();
  const vcl_ptrdiff_t s_kstep = src_image.kstep();
  const vcl_ptrdiff_t s_pstep = src_image.planestep();
  const T* s_plane = src_image.origin_ptr();

  const unsigned dni = static_cast<unsigned>(sni*dx);
  const unsigned dnj = static_cast<unsigned>(snj*dy);
  const unsigned dnk = static_cast<unsigned>(snk*dz);
  dst_image.set_size(dni, dnj, dnk, np);
  const vcl_ptrdiff_t d_istep = dst_image.istep();
  const vcl_ptrdiff_t d_jstep = dst_image.jstep();
  const vcl_ptrdiff_t d_kstep = dst_image.kstep();
  const vcl_ptrdiff_t d_pstep = dst_image.planestep();
  T* d_plane = dst_image.origin_ptr();

  // Use this to convert from double to T with appropriate rounding
  vil_convert_round_pixel<double, T> cr;

  // Loop over all voxels in the destination image
  // and sample from the corresponding point in the source image
  // (except near upper boundaries).
  for (unsigned p=0; p<np; ++p, s_plane+=s_pstep, d_plane+=d_pstep)
  {
    T* d_slice = d_plane;
    for (unsigned k=0; k<static_cast<unsigned>(dnk-dz); ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=0; j<static_cast<unsigned>(dnj-dy); ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<static_cast<unsigned>(dni-dx); ++i, d_pix+=d_istep)
        {
          cr(vil3d_trilin_interp_raw(i/dx, j/dy, k/dz,
                                     s_plane,
                                     s_istep, s_jstep, s_kstep),
             *d_pix);
        }
        // Process the pixels near the upper i boundary - safe_extend interpolation
        for (unsigned i=static_cast<unsigned>(dni-dx); i<dni; ++i, d_pix+=d_istep)
        {
          cr(vil3d_trilin_interp_safe_extend(i/dx, j/dy, k/dz,
                                             s_plane,
                                             sni, snj, snk,
                                             s_istep, s_jstep, s_kstep),
             *d_pix);
        }
      }

      // Process the pixels near the upper j boundary - safe_extend interpolation
      for (unsigned j=static_cast<unsigned>(dnj-dy); j<dnj; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<dni; ++i, d_pix+=d_istep)
        {
          cr(vil3d_trilin_interp_safe_extend(i/dx, j/dy, k/dz,
                                             s_plane,
                                             sni, snj, snk,
                                             s_istep, s_jstep, s_kstep),
                                             *d_pix);
        }
      }
    }

    // Process the pixels near the upper k boundary - safe_extend interpolation
    for (unsigned k=static_cast<unsigned>(dnk-dz); k<dnk; ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=0; j<dnj; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<dni; ++i, d_pix+=d_istep)
        {
          cr(vil3d_trilin_interp_safe_extend(i/dx, j/dy, k/dz,
                                             s_plane,
                                             sni, snj, snk,
                                             s_istep, s_jstep, s_kstep),
             *d_pix);
        }
      }
    }
  }
}


template <class T>
void vil3d_resample_trilinear_scale_2(
  const vil3d_image_view<T>& src_im,
  vil3d_image_view<T>& dest_im)
{
  // Assume planes are the same for both images
  const unsigned np = src_im.nplanes();

  const unsigned ni = src_im.ni();
  const unsigned nj = src_im.nj();
  const unsigned nk = src_im.nk();
  dest_im.set_size(ni*2-1, nj*2-1, nk*2-1, np);

  for (unsigned p = 0; p<np; ++p)
  {
    const vil3d_image_view<T> src = vil3d_plane(src_im, p);
    vil3d_image_view<T> dest = vil3d_plane(dest_im, p);

    for (unsigned k=0; k<nk-1; ++k)
    {
      for (unsigned j=0; j<nj-1; ++j)
      {
        // Do all except last slice.
        for (unsigned i=0; i<ni-1; ++i)
        {
          // s0-s7 are the values at 8 neighbouring positions (on a cube) in src.
          // d0-d7 are the values at 8 neighbouring positions (on a cube of 1/8 the above cube's size) in dest.
          // They are aligned so that s0 = d0.
          // d6t2 is the value of d6 time 2, etc.
          T s0 = src(i,j,k);
          T s1 = src(i+1, j  , k  );
          T s2 = src(i,   j+1, k  );
          T s3 = src(i+1, j+1, k  );
          T s4 = src(i  , j  , k+1);
          T s5 = src(i+1, j  , k+1);
          T s6 = src(i  , j+1, k+1);
          T s7 = src(i+1, j+1, k+1);
                                    dest(2*i  , 2*j  , 2*k  ) = s0;
          T d1t2 = s0+s1;           dest(2*i+1, 2*j  , 2*k  ) = d1t2/2;
          T d2t2 = s0+s2;           dest(2*i  , 2*j+1, 2*k  ) = d2t2/2;
          T d4t2 = s0+s4;           dest(2*i  , 2*j  , 2*k+1) = d4t2/2;
          T d3t4 = d2t2 + s1+s3;    dest(2*i+1, 2*j+1, 2*k  ) = d3t4/4;
          T d5t4 = d4t2 + s1+s5;    dest(2*i+1, 2*j  , 2*k+1) = d5t4/4;
          T d6t4 = d4t2 + s2+s6;    dest(2*i  , 2*j+1, 2*k+1) = d6t4/4;
                                    dest(2*i+1, 2*j+1, 2*k+1) = (d6t4 + s1+s3+s5+s7)/8;
        }
        T s0 = src(ni-1, j  , k  );
        T s2 = src(ni-1, j+1, k  );
        T s4 = src(ni-1, j  , k+1);
        T s6 = src(ni-1, j+1, k+1);
        dest(ni*2-2, j*2  , k*2  ) = s0;
        dest(ni*2-2, j*2+1, k*2  ) = (s0 + s2)/2;
        dest(ni*2-2, j*2  , k*2+1) = (s0 + s4)/2;
        dest(ni*2-2, j*2+1, k*2+1) = (s0 + s2 + s4 + s6)/4;
      }
      // Do last plane
      for (unsigned i=0; i<ni-1; ++i)
      {
        T s0 = src(i  , nj-1, k  );
        T s1 = src(i+1, nj-1, k  );
        T s4 = src(i  , nj-1, k+1);
        T s5 = src(i+1, nj-1, k+1);
        dest(i*2  , nj*2-2, k*2  ) = s0;
        dest(i*2+1, nj*2-2, k*2  ) = (s0 + s1)/2;
        dest(i*2  , nj*2-2, k*2+1) = (s0 + s4)/2;
        dest(i*2+1, nj*2-2, k*2+1) = (s0 + s1 + s4 + s5)/4;
      }
      T s0 = src(ni-1, nj-1, k);
      dest(ni*2-2, nj*2-2, k*2  ) = s0;
      dest(ni*2-2, nj*2-2, k*2+1) = (s0 + src(ni-1, nj-1, k+1))/2;
    }
    // Do last plane
    for (unsigned j=0; j<nj-1; ++j)
    {
      for (unsigned i=0; i<ni-1; ++i)
      {
        T s0 = src(i  , j  , nk-1);
        T s1 = src(i+1, j  , nk-1);
        T s2 = src(i  , j+1, nk-1);
        T s3 = src(i+1, j+1, nk-1);
        dest(i*2  , j*2  , nk*2-2) = s0;
        dest(i*2+1, j*2  , nk*2-2) = (s0 + s1)/2;
        dest(i*2  , j*2+1, nk*2-2) = (s0 + s2)/2;
        dest(i*2+1, j*2+1, nk*2-2) = (s0 + s1 + s2 + s3)/4;
      }
      T s0 = src(ni-1, j, nk-1);
      dest(ni*2-2, j*2  , nk*2-2) = s0;
      dest(ni*2-2, j*2+1, nk*2-2) = (s0 + src(ni-1, j+1, nk-1))/2;
    }
    for (unsigned i=0; i<ni-1; ++i)
    {
      T s0 = src(i, nj-1, nk-1);
      dest(i*2  , nj*2-2, nk*2-2) = s0;
      dest(i*2+1, nj*2-2, nk*2-2) = (s0 + src(i+1, nj-1, nk-1))/2;
    }
    dest(ni*2-2, nj*2-2, nk*2-2) = src(ni-1, nj-1, nk-1);
  }
}


#define VIL3D_RESAMPLE_TRILINEAR_INSTANTIATE( T, S ) \
template void vil3d_resample_trilinear(const vil3d_image_view<S>& src_image, \
                              vil3d_image_view<T>& dest_image, \
                              double x0, double y0, double z0, \
                              double dx1, double dy1, double dz1, \
                              double dx2, double dy2, double dz2, \
                              double dx3, double dy3, double dz3, \
                              int n1, int n2, int n3); \
template void vil3d_resample_trilinear(const vil3d_image_view<S>& src_image, \
                              vil3d_image_view<T>& dest_image, \
                              int n1, int n2, int n3); \
template void vil3d_resample_trilinear(const vil3d_image_view< T >& src_image, \
                                       vil3d_image_view< T >& dst_image, \
                                       const double dx, \
                                       const double dy, \
                                       const double dz); \
template void vil3d_resample_trilinear_scale_2(const vil3d_image_view< T >& src_image, \
                                       vil3d_image_view< T >& dst_image)

#endif // vil3d_resample_trilinear_txx_
