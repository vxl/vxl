// This is core/vil/vil_sample_grid_bicub.hxx
#ifndef vil_sample_grid_bicub_hxx_
#define vil_sample_grid_bicub_hxx_
//:
// \file
// \brief Bicubic profile sampling functions for 2D images
//
// The vil bicub source files were derived from the corresponding
// vil bilin files, thus the vil bilin/bicub source files are very
// similar.  If you modify something in this file, there is a
// corresponding bilin file that would likely also benefit from
// the same change.

#include "vil_sample_grid_bicub.h"
#include <vil/vil_bicub_interp.h>

//: This function should not be the same in bicub and bilin
inline bool vil_grid_bicub_corner_in_image(double x0, double y0,
                                           const vil_image_view_base& image)
{
  return x0 >= 2
      && y0 >= 2
      && x0+3 <= image.ni()
      && y0+3 <= image.nj();
}

//: Sample along profile, using safe bicubic interpolation
//  Profile points are along the line between p0 and p1 (in image co-ordinates).
//  Vector v is resized to n*np elements, where np=image.n_planes().
//  v[0]..v[np-1] are the values from point p
//  Points outside image return zero.
template <class imType, class vecType>
void vil_sample_grid_bicub(vecType* v,
                           const vil_image_view<imType>& image,
                           double x0, double y0, double dx1, double dy1,
                           double dx2, double dy2, int n1, int n2)
{
  bool all_in_image =    vil_grid_bicub_corner_in_image(x0,y0,image)
                      && vil_grid_bicub_corner_in_image(x0+(n1-1)*dx1,y0+(n1-1)*dy1,image)
                      && vil_grid_bicub_corner_in_image(x0+(n2-1)*dx2,y0+(n2-1)*dy2,image)
                      && vil_grid_bicub_corner_in_image(x0+(n1-1)*dx1+(n2-1)*dx2,
                                                        y0+(n1-1)*dy1+(n2-1)*dy2,image);

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned np = image.nplanes();
  const std::ptrdiff_t istep = image.istep();
  const std::ptrdiff_t jstep = image.jstep();
  const std::ptrdiff_t pstep = image.planestep();
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
          *v = (vecType) vil_bicub_interp(x,y,plane0,ni,nj,istep,jstep);
      }
    }
    else
    {
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
      {
        double x=x1, y=y1; // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2)
        {
          for (unsigned p=0;p<np;++p,++v)
            *v = (vecType) vil_bicub_interp(x,y,plane0+p*pstep,ni,nj,istep,jstep);
        }
      }
    }
  }
  else
  {
    // Use safe interpolation
    if (np==1)
    {
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
      {
        double x=x1, y=y1;  // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2,++v)
          *v = (vecType) vil_bicub_interp_safe(x,y,plane0,ni,nj,istep,jstep);
      }
    }
    else
    {
      for (int i=0;i<n1;++i,x1+=dx1,y1+=dy1)
      {
        double x=x1, y=y1; // Start of j-th row
        for (int j=0;j<n2;++j,x+=dx2,y+=dy2)
        {
          for (unsigned p=0;p<np;++p,++v)
            *v = (vecType) vil_bicub_interp_safe(x,y,plane0+p*pstep,ni,nj,istep,jstep);
        }
      }
    }
  }
}

#define VIL_SAMPLE_GRID_BICUB_INSTANTIATE( imType, vecType ) \
template void vil_sample_grid_bicub(vecType* v, \
                                    const vil_image_view<imType >& image, \
                                    double x0, double y0, double dx1, double dy1, \
                                    double dx2, double dy2, int n1, int n2)

#endif // vil_sample_grid_bicub_hxx_
