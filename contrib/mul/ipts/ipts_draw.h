#ifndef ipts_draw_h_
#define ipts_draw_h_

//:
//  \file
//  \brief Functions to draw shapes into an image (may get moved to vil in time)
//  \author Tim Cootes

#include <vil/vil_image_view.h>

//: Draws line between (i0,j0) and (i0+L,j0) (which may be outside image)
//  Assumes single plane
template<class T>
inline void ipts_draw_i_line(vil_image_view<T>& image, 
                             int i0, int j0, unsigned L, T value)
{
  int ilo = i0; if (ilo<0) ilo=0;
  int ihi = i0+L; if (ihi>=int(image.ni()))  ihi=image.ni()-1;
  for (int i=ilo;i<=ihi;++i) image(i,j0)=value;
}

//: Draws line between (i0,j0) and (i0,j0+L) (which may be outside image)
//  Assumes single plane
template<class T>
inline void ipts_draw_j_line(vil_image_view<T>& image, 
                             int i0, int j0, unsigned L, T value)
{
  int jlo = j0; if (jlo<0) jlo=0;
  int jhi = j0+L; if (jhi>=int(image.nj()))  jhi=image.nj()-1;
  for (int j=jlo;j<=jhi;++j) image(i0,j)=value;
}

//: Draw cross centred at (i0,j0) with half-width L
template<class T>
void ipts_draw_cross(vil_image_view<T>& image, int i0, int j0, unsigned L, T value)
{
  ipts_draw_i_line(image,i0-L,j0,2*L+1,value);
  ipts_draw_j_line(image,i0,j0-L,2*L+1,value);
}


#endif // ipts_draw_h_
