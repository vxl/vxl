// This is mul/vil2/vil2_resample_bilin.txx
#ifndef vil2_resample_bilin_txx_
#define vil2_resample_bilin_txx_
//: \file
//  \brief Sample grid of points in one image and place in another
//  \author Tim Cootes

#include "vil2_resample_bilin.h"
#include <vil2/vil2_bilin_interp.h>

inline bool vil2_grid_corner_in_image(double x0, double y0,
                                      const vil2_image_view_base& image)
{
  if (x0<1) return false;
  if (y0<1) return false;
  if (x0+2>image.ni()) return false;
  if (y0+2>image.nj()) return false;
  return true;
}

//: Sample grid of points in one image and place in another, using bilinear interpolation
//  dest_image(i,j,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  dest_image resized to (n1,n2,src_image.nplanes())
//  Points outside image return zero.
// \relates vil2_image_view
template <class sType, class dType>
void vil2_resample_bilin(const vil2_image_view<sType>& src_image,
                         vil2_image_view<dType>& dest_image,
                         double x0, double y0, double dx1, double dy1,
                         double dx2, double dy2, int n1, int n2)
{
  bool all_in_image =    vil2_grid_corner_in_image(x0,y0,src_image)
                      && vil2_grid_corner_in_image(x0+(n1-1)*dx1,y0+(n1-1)*dy1,src_image)
                      && vil2_grid_corner_in_image(x0+(n2-1)*dx2,y0+(n2-1)*dy2,src_image)
                      && vil2_grid_corner_in_image(x0+(n1-1)*dx1+(n2-1)*dx2,
                                                   y0+(n1-1)*dy1+(n2-1)*dy2,src_image);

  const unsigned ni = src_image.ni();
  const unsigned nj = src_image.nj();
  const unsigned np = src_image.nplanes();
  const int istep = src_image.istep();
  const int jstep = src_image.jstep();
  const int pstep = src_image.planestep();
  const sType* plane0 = src_image.top_left_ptr();

	dest_image.resize(n1,n2,np);
  const int d_istep = dest_image.istep();
  const int d_jstep = dest_image.jstep();
  const int d_pstep = dest_image.planestep();
  dType* d_plane0 = dest_image.top_left_ptr();

  double x1=x0;
  double y1=y0;

  if (all_in_image)
  {
    if (np==1)
    {
		  dType *row = d_plane0;
      for (int j=0;j<n2;++j,x1+=dx2,y1+=dy2,row+=d_jstep)
      {
        double x=x1, y=y1;  // Start of j-th row
		    dType *dpt = row;
        for (int i=0;i<n1;++i,x+=dx1,y+=dy1,dpt+=d_istep)
          *dpt = (dType) vil2_bilin_interp_raw(x,y,plane0,istep,jstep);
      }
    }
    else
    {
		  dType *row = d_plane0;
      for (int j=0;j<n2;++j,x1+=dx2,y1+=dy2,row+=d_jstep)
      {
        double x=x1, y=y1; // Start of j-th row
		    dType *dpt = row;
        for (int i=0;i<n1;++i,x+=dx1,y+=dy1,dpt+=d_istep)
        {
          for (int p=0;p<np;++p)
            dpt[p*d_pstep] = (dType) vil2_bilin_interp_raw(x,y,plane0+p*pstep,istep,jstep);
        }
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
		  dType *row = d_plane0;
      for (int j=0;j<n2;++j,x1+=dx2,y1+=dy2,row+=d_jstep)
      {
        double x=x1, y=y1;  // Start of j-th row
		    dType *dpt = row;
        for (int i=0;i<n1;++i,x+=dx1,y+=dy1,dpt+=d_istep)
          *dpt = (dType) vil2_bilin_interp_safe(x,y,plane0,
					                                      ni,nj,istep,jstep);
      }
    }
    else
    {
		  dType *row = d_plane0;
      for (int j=0;j<n2;++j,x1+=dx2,y1+=dy2,row+=d_jstep)
      {
        double x=x1, y=y1; // Start of j-th row
		    dType *dpt = row;
        for (int i=0;i<n1;++i,x+=dx1,y+=dy1,dpt+=d_istep)
        {
          for (int p=0;p<np;++p)
            dpt[p*d_pstep] = (dType) vil2_bilin_interp_safe(x,y,plane0+p*pstep,
						                                                ni,nj,istep,jstep);
        }
      }
    }
  }
}

#define VIL2_RESAMPLE_BILIN_INSTANTIATE( sType, dType ) \
template void vil2_resample_bilin(const vil2_image_view<sType >& src_image, \
                         vil2_image_view<dType >& dest_image, \
                         double x0, double y0, double dx1, double dy1, \
                         double dx2, double dy2, int n1, int n2)

#endif // vil2_resample_bilin
