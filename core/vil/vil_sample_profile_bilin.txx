// This is mul/vil2/vil2_sample_profile_bilin.txx
#ifndef vil2_sample_profile_bilin_txx_
#define vil2_sample_profile_bilin_txx_
//: \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include <vil2/vil2_sample_profile_bilin.h>
#include <vil2/vil2_bilin_interp.h>

inline bool vil2_profile_in_image(double x0, double y0,
                                  double x1, double y1,
                                  const vil2_image_view_base& image)
{
  if (x0<1) return false;
  if (y0<1) return false;
  if (x0+2>image.ni()) return false;
  if (y0+2>image.nj()) return false;
  if (x1<1) return false;
  if (y1<1) return false;
  if (x1+2>image.ni()) return false;
  if (y1+2>image.nj()) return false;

  return true;
}

//: Sample along profile, using safe bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void vil2_sample_profile_bilin(vecType* v,
                               const vil2_image_view<imType>& image,
                               double x0, double y0, double dx, double dy,
                               int n)
{
  bool all_in_image = vil2_profile_in_image(x0,y0,x0+(n-1)*dx,y0+(n-1)*dy,image);

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned np = image.nplanes();
  const int istep = image.istep();
  const int jstep = image.jstep();
  const int pstep = image.planestep();
  double x=x0;
  double y=y0;
  const imType* plane0 = image.top_left_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      v[k] = vil2_bilin_interp(x,y,plane0,ni,nj,istep,jstep);
    }
    else
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      {
        for (int p=0;p<np;++p,++v)
          *v = vil2_bilin_interp(x,y,plane0+p*pstep,ni,nj,istep,jstep);
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      v[k] = vil2_bilin_interp_safe(x,y,plane0,ni,nj,istep,jstep);
    }
    else
    {
      for (int k=0;k<n;++k,x+=dx,y+=dy)
      {
        for (int p=0;p<np;++p,++v)
          *v = vil2_bilin_interp_safe(x,y,plane0+p*pstep,ni,nj,istep,jstep);
      }
    }
  }
}

#define VIL2_SAMPLE_PROFILE_BILIN_INSTANTIATE( imType, vecType ) \
template void vil2_sample_profile_bilin(vecType* v, \
                                        const vil2_image_view<imType >& image, \
                                        double x0, double y0, \
                                        double dx, double dy, \
                                        int n)

#endif // vil2_sample_profile_bilin
