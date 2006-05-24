// This is mul/vil3d/vil3d_sample_profile_trilin.txx
#ifndef vil3d_sample_profile_trilin_txx_
#define vil3d_sample_profile_trilin_txx_
//:
// \file
// \brief Profile sampling functions for 3D images
// \author Tim Cootes

#include <vil3d/vil3d_sample_profile_trilin.h>
#include <vil3d/vil3d_trilin_interp.h>

inline bool vil3d_profile_in_image(double x0, double y0, double z0,
                                   double x1, double y1,double z1,
                                   const vil3d_image_view_base& image)
{
  if (x0<1) return false;
  if (y0<1) return false;
  if (z0<1) return false;
  if (x0+2>image.ni()) return false;
  if (y0+2>image.nj()) return false;
  if (z0+2>image.nk()) return false;
  if (x1<1) return false;
  if (y1<1) return false;
  if (z1<1) return false;
  if (x1+2>image.ni()) return false;
  if (y1+2>image.nj()) return false;
  if (z1+2>image.nk()) return false;

  return true;
}

//: Sample along profile, using safe trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void vil3d_sample_profile_trilin(
  vecType* v,
  const vil3d_image_view<imType>& image,
  double x0, double y0, double z0,
  double dx, double dy, double dz,
  unsigned n)
{
  bool all_in_image = vil3d_profile_in_image(x0,y0,z0,
                                             x0+(n-1)*dx,y0+(n-1)*dy,z0+(n-1)*dz,
                                             image);

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned nk = image.nk();
  const unsigned np = image.nplanes();
  const vcl_ptrdiff_t istep = image.istep();
  const vcl_ptrdiff_t jstep = image.jstep();
  const vcl_ptrdiff_t kstep = image.kstep();
  const vcl_ptrdiff_t pstep = image.planestep();
  double x=x0;
  double y=y0;
  double z=z0;
  const imType* plane0 = image.origin_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      v[h] = vil3d_trilin_interp_raw(x,y,z,plane0,istep,jstep,kstep);
    }
    else
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      {
        for (unsigned p=0;p<np;++p,++v)
          *v = vil3d_trilin_interp_raw(x,y,z,plane0+p*pstep,istep,jstep,kstep);
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      v[h] = vil3d_trilin_interp_safe(x,y,z,plane0,ni,nj,nk,istep,jstep,kstep);
    }
    else
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      {
        for (unsigned p=0;p<np;++p,++v)
          *v = vil3d_trilin_interp_safe(x,y,z,plane0+p*pstep,ni,nj,nk,istep,jstep,kstep);
      }
    }
  }
}

//: Sample along profile, using safe-extend trilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void vil3d_sample_profile_trilin_extend(
  vecType* v,
  const vil3d_image_view<imType>& image,
  double x0, double y0, double z0,
  double dx, double dy, double dz,
  unsigned n)
{
  bool all_in_image = vil3d_profile_in_image(x0,y0,z0,
                                             x0+(n-1)*dx,y0+(n-1)*dy,z0+(n-1)*dz,
                                             image);

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned nk = image.nk();
  const unsigned np = image.nplanes();
  const vcl_ptrdiff_t istep = image.istep();
  const vcl_ptrdiff_t jstep = image.jstep();
  const vcl_ptrdiff_t kstep = image.kstep();
  const vcl_ptrdiff_t pstep = image.planestep();
  double x=x0;
  double y=y0;
  double z=z0;
  const imType* plane0 = image.origin_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      v[h] = vil3d_trilin_interp_raw(x,y,z,plane0,istep,jstep,kstep);
    }
    else
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      {
        for (unsigned p=0;p<np;++p,++v)
          *v = vil3d_trilin_interp_raw(x,y,z,plane0+p*pstep,istep,jstep,kstep);
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      v[h] = vil3d_trilin_interp_safe_extend(x,y,z,plane0,ni,nj,nk,istep,jstep,kstep);
    }
    else
    {
      for (unsigned h=0;h<n;++h,x+=dx,y+=dy,z+=dz)
      {
        for (unsigned p=0;p<np;++p,++v)
          *v = vil3d_trilin_interp_safe_extend(x,y,z,plane0+p*pstep,ni,nj,nk,istep,jstep,kstep);
      }
    }
  }
}


#define VIL3D_SAMPLE_PROFILE_TRILIN_INSTANTIATE( imType, vecType ) \
template void vil3d_sample_profile_trilin(vecType* v, \
  const vil3d_image_view<imType >& image, \
  double x0, double y0, double z0,\
  double dx, double dy, double dz,\
  unsigned n); \
template void vil3d_sample_profile_trilin_extend(vecType* v, \
  const vil3d_image_view<imType >& image, \
  double x0, double y0, double z0,\
  double dx, double dy, double dz,\
  unsigned n)
#endif // vil3d_sample_profile_trilin_txx_
