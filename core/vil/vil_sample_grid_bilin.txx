#ifndef vil2_sample_profile_bilin_txx_
#define vil2_sample_profile_bilin_txx_

//: \file
//  \brief Profile sampling functions for 2D images
//  \author Tim Cootes

#include "vil2_sample_grid_bilin.h"
#include <vil2/vil2_bilin_interp.h>

inline bool vil2_grid_corner_in_image(double x0, double y0,
                                  const vil2_image_view_base& image)
{
  if (x0<1) return false;
  if (y0<1) return false;
  if (x0+2>image.nx()) return false;
  if (y0+2>image.ny()) return false;
  return true;
}

//: Sample along profile, using safe bilinear interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void vil2_sample_grid_bilin(vecType* v,
                           const vil2_image_view<imType>& image,
                           double x0, double y0, double dx1, double dy1,
                           double dx2, double dy2, int n1, int n2)
{
  bool all_in_image = vil2_grid_corner_in_image(x0,y0,image)
                      && vil2_grid_corner_in_image(x0+(n1-1)*dx1,y0+(n1-1)*dy1,image)
                      && vil2_grid_corner_in_image(x0+(n2-1)*dx2,y0+(n2-1)*dy2,image)
                      && vil2_grid_corner_in_image(x0+(n1-1)*dx1+(n2-1)*dx2,
					                               y0+(n1-1)*dy1+(n2-1)*dy2,image);

  int np = image.nplanes();
  int xstep = image.xstep();
  int ystep = image.ystep();
  int pstep = image.planestep();
  double x1=x0;
  double y1=y0;
  const imType* plane0 = image.top_left_ptr();

  if (all_in_image)
  {
    if (np==1)
    {
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
	  {
	    double x=x1, y=y1;  // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2,++v)
          *v = vil2_bilin_interp(x,y,plane0,xstep,ystep);
      }
    }
    else
    {
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
	  {
	    double x=x1, y=y1; // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2)
        {
          for (int k=0;k<np;++k,++v)
            *v = vil2_bilin_interp(x,y,plane0+k*pstep,xstep,ystep);
        }
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
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
	  {
	    double x=x1, y=y1;  // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2,++v)
          *v = vil2_safe_bilin_interp(x,y,plane0,nx,ny,xstep,ystep);
      }
    }
    else
    {
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
	  {
	    double x=x1, y=y1; // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2)
        {
          for (int k=0;k<np;++k,++v)
            *v = vil2_safe_bilin_interp(x,y,plane0+k*pstep,nx,ny,xstep,ystep);
        }
      }
    }
  }
}

#define VIL2_SAMPLE_GRID_BILIN_INSTANTIATE( imType, vecType ) \
template void vil2_sample_grid_bilin(vecType* v, \
                           const vil2_image_view<imType >& image, \
                           double x0, double y0, double dx1, double dy1, \
                           double dx2, double dy2, int n1, int n2)

#endif // vil2_sample_grid_bilin
