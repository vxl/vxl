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
  if (x0+2>image.nx()) return false;
  if (y0+2>image.ny()) return false;
  if (x1<1) return false;
  if (y1<1) return false;
  if (x1+2>image.nx()) return false;
  if (y1+2>image.ny()) return false;

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

  int np = image.nplanes();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int pstep = image.planestep();
  double x=x0;
  double y=y0;
  const imType* plane0 = image.top_left_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (int i=0;i<n;++i,x+=dx,y+=dy)
      v[i] = vil2_bilin_interp(x,y,plane0,xstep,ystep);
    }
    else
    {
      for (int i=0;i<n;++i,x+=dx,y+=dy)
      {
        for (int j=0;j<np;++j,++v)
          *v = vil2_bilin_interp(x,y,plane0+j*pstep,xstep,ystep);
      }
    }
  }
  else
  {
    // Use safe interpolation
	int nx = image.nx();
	int ny = image.ny();
    if (np==1)
    {
      for (int i=0;i<n;++i,x+=dx,y+=dy)
      v[i] = vil2_safe_bilin_interp(x,y,plane0,nx,ny,xstep,ystep);
    }
    else
    {
      for (int i=0;i<n;++i,x+=dx,y+=dy)
      {
        for (int j=0;j<np;++j,++v)
          *v = vil2_safe_bilin_interp(x,y,plane0+j*pstep,nx,ny,xstep,ystep);
      }
    }
  }
}

#define VIL2_SAMPLE_PROFILE_BILIN_INSTANTIATE( imType, vecType ) \
template void vil2_sample_profile_bilin(vecType* v, \
                           const vil2_image_view<imType >& image, \
                           double x0, double y0, double dx, double dy, \
                           int n)

#endif // vil2_sample_profile_bilin
