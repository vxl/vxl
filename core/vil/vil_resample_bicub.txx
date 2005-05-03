// This is core/vil/vil_resample_bicub.txx
#ifndef vil_resample_bicub_txx_
#define vil_resample_bicub_txx_
//:
// \file
// \brief Sample grid of points with bicubic interpolation in one image and place in another
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin file that would likely also benefit from
// the same change.

#include "vil_resample_bicub.h"
#include <vil/vil_bicub_interp.h>

inline bool vil_grid_corner_in_image(double x0, double y0,
                                     const vil_image_view_base& image)
{
  if (x0<1) return false;
  if (y0<1) return false;
  if (x0>image.ni()-2) return false;
  if (y0>image.nj()-2) return false;
  return true;
}

//: Sample grid of points in one image and place in another, using bicubic interpolation.
//  dest_image(i,j,p) is sampled from the src_image at
//  (x0+i.dx1+j.dx2,y0+i.dy1+j.dy2), where i=[0..n1-1], j=[0..n2-1]
//  dest_image resized to (n1,n2,src_image.nplanes())
//  Points outside image return zero.
// \relates vil_image_view
template <class sType, class dType>
void vil_resample_bicub(const vil_image_view<sType>& src_image,
                        vil_image_view<dType>& dest_image,
                        double x0, double y0, double dx1, double dy1,
                        double dx2, double dy2, int n1, int n2)
{
  bool all_in_image =    vil_grid_corner_in_image(x0,y0,src_image)
                      && vil_grid_corner_in_image(x0+(n1-1)*dx1,y0+(n1-1)*dy1,src_image)
                      && vil_grid_corner_in_image(x0+(n2-1)*dx2,y0+(n2-1)*dy2,src_image)
                      && vil_grid_corner_in_image(x0+(n1-1)*dx1+(n2-1)*dx2,
                                                  y0+(n1-1)*dy1+(n2-1)*dy2,src_image);

  const unsigned ni = src_image.ni();
  const unsigned nj = src_image.nj();
  const unsigned np = src_image.nplanes();
  const vcl_ptrdiff_t istep = src_image.istep();
  const vcl_ptrdiff_t jstep = src_image.jstep();
  const vcl_ptrdiff_t pstep = src_image.planestep();
  const sType* plane0 = src_image.top_left_ptr();

  dest_image.set_size(n1,n2,np);
  const vcl_ptrdiff_t d_istep = dest_image.istep();
  const vcl_ptrdiff_t d_jstep = dest_image.jstep();
  const vcl_ptrdiff_t d_pstep = dest_image.planestep();
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
          *dpt = (dType) vil_bicub_interp_raw(x,y,plane0,ni,nj,istep,jstep);
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
          for (unsigned int p=0;p<np;++p)
            dpt[p*d_pstep] = (dType) vil_bicub_interp_raw(x,y,plane0+p*pstep,ni,nj,istep,jstep);
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
          *dpt = (dType) vil_bicub_interp_safe(x,y,plane0,
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
          for (unsigned int p=0;p<np;++p)
            dpt[p*d_pstep] = (dType) vil_bicub_interp_safe(x,y,plane0+p*pstep,
                                                            ni,nj,istep,jstep);
        }
      }
    }
  }
}


//: Resample image to a specified width (n1) and height (n2)
template <class sType, class dType>
void vil_resample_bicub(const vil_image_view<sType>& src_image,
                        vil_image_view<dType>& dest_image,
                        int n1, int n2)
{
  double f= 0.9999999; // so sampler doesn't go off edge of image
  double x0=0;
  double y0=0;
  double dx1=f*(src_image.ni()-1)*1.0/(n1-1);
  double dy1=0;
  double dx2=0;
  double dy2=f*(src_image.nj()-1)*1.0/(n2-1);
  vil_resample_bicub( src_image, dest_image, x0, y0, dx1, dy1, dx2, dy2, n1, n2 );
}

#define VIL_RESAMPLE_BICUB_INSTANTIATE( sType, dType ) \
template void vil_resample_bicub(const vil_image_view<sType >& src_image, \
                                 vil_image_view<dType >& dest_image, \
                                 double x0, double y0, double dx1, double dy1, \
                                 double dx2, double dy2, int n1, int n2); \
template void vil_resample_bicub(const vil_image_view<sType >& src_image, \
                                 vil_image_view<dType >& dest_image, \
                                 int n1, int n2)

#endif // vil_resample_bicub_txx_
