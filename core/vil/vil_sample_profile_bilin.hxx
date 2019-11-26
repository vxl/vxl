// This is core/vil/vil_sample_profile_bilin.hxx
#ifndef vil_sample_profile_bilin_hxx_
#define vil_sample_profile_bilin_hxx_
//:
// \file
// \brief Bilinear profile sampling functions for 2D images
// \author Tim Cootes
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bicub file that would likely also benefit from
// the same change.

#include "vil_sample_profile_bilin.h"
#include "vil_bilin_interp.h"

//: This function should not be the same in bicub and bilin
inline bool vil_profile_bilin_in_image(double x0, double y0,
                                       double x1, double y1,
                                       const vil_image_view_base& image)
{
  return x0 >= 1
      && y0 >= 1
      && x1 >= 1
      && y1 >= 1
      && x0+2 <= image.ni()
      && y0+2 <= image.nj()
      && x1+2 <= image.ni()
      && y1+2 <= image.nj();
}

//: Sample along profile, using safe bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void vil_sample_profile_bilin(vecType* v,
                              const vil_image_view<imType>& image,
                              double x0, double y0, double dx, double dy,
                              int n)
{
  bool all_in_image = vil_profile_bilin_in_image(x0,y0,x0+(n-1)*dx,y0+(n-1)*dy,image);

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned np = image.nplanes();
  const std::ptrdiff_t istep = image.istep();
  const std::ptrdiff_t jstep = image.jstep();
  const std::ptrdiff_t pstep = image.planestep();
  double x=x0;
  double y=y0;
  const imType* plane0 = image.top_left_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      v[k] = vil_bilin_interp(x,y,plane0,ni,nj,istep,jstep);
    }
    else
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      {
        for (unsigned int p=0;p<np;++p,++v)
          *v = vil_bilin_interp(x,y,plane0+p*pstep,ni,nj,istep,jstep);
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      v[k] = vil_bilin_interp_safe(x,y,plane0,ni,nj,istep,jstep);
    }
    else
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      {
        for (unsigned int p=0;p<np;++p,++v)
          *v = vil_bilin_interp_safe(x,y,plane0+p*pstep,ni,nj,istep,jstep);
      }
    }
  }
}

//: Sample along profile, using bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return NA.
template <class imType, class vecType>
void vil_sample_profile_bilin_edgena(vecType* v,
                                     const vil_image_view<imType>& image,
                                     double x0, double y0, double dx, double dy,
                                     int n)
{
  bool all_in_image = vil_profile_bilin_in_image(x0,y0,x0+(n-1)*dx,y0+(n-1)*dy,image);

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned np = image.nplanes();
  const std::ptrdiff_t istep = image.istep();
  const std::ptrdiff_t jstep = image.jstep();
  const std::ptrdiff_t pstep = image.planestep();
  double x=x0;
  double y=y0;
  const imType* plane0 = image.top_left_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      v[k] = vil_bilin_interp(x,y,plane0,ni,nj,istep,jstep);
    }
    else
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      {
        for (unsigned int p=0;p<np;++p,++v)
          *v = vil_bilin_interp(x,y,plane0+p*pstep,ni,nj,istep,jstep);
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
        v[k] = vil_bilin_interp_safe_edgena(x,y,plane0,ni,nj,istep,jstep);
    }
    else
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      {
        for (unsigned int p=0;p<np;++p,++v)
          *v = vil_bilin_interp_safe_edgena(x,y,plane0+p*pstep,ni,nj,istep,jstep);
      }
    }
  }
}

#define VIL_SAMPLE_PROFILE_BILIN_INSTANTIATE( imType, vecType ) \
template void vil_sample_profile_bilin(vecType* v, \
                                       const vil_image_view<imType >& image, \
                                       double x0, double y0, \
                                       double dx, double dy, \
                                       int n); \
template void vil_sample_profile_bilin_edgena(vecType* v, \
                                              const vil_image_view<imType >& image, \
                                              double x0, double y0, \
                                              double dx, double dy, \
                                              int n)
#endif // vil_sample_profile_bilin_hxx_
