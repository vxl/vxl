#ifndef mbl_draw_line_h_
#define mbl_draw_line_h_

//:
//  \file
//  \brief Functions to draw shapes into an image (may get moved to vil in time)
//  \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_algorithm.h>
#include <vcl_cmath.h>

//: Draws value along line between p1 and p2
//  Effective, but not terribly efficient.
template<class T>
inline void mbl_draw_line(vil_image_view<T>& image,
               vgl_point_2d<double> p1,
               vgl_point_2d<double> p2, T value)
{
  vgl_vector_2d<double> dp = p2-p1;
  unsigned n = unsigned(1.5+vcl_max(vcl_fabs(dp.x()),vcl_fabs(dp.y())));
  dp/=n;
  unsigned ni=image.ni(), nj=image.nj();
  for (unsigned i=0;i<=n;++i,p1+=dp)
  {
    unsigned pi=unsigned(p1.x()+0.5); if (pi>=ni) continue;
    unsigned pj=unsigned(p1.y()+0.5); if (pj>=nj) continue;
    image(pi,pj)=value;
  }
}

//: Draws colour (r,g,b) along line between p1 and p2 in 3-plane image
//  Effective, but not terribly efficient.
template<class T>
inline void mbl_draw_line(vil_image_view<T>& image,
               vgl_point_2d<double> p1,
               vgl_point_2d<double> p2, 
               T r, T g, T b)
{
  vgl_vector_2d<double> dp = p2-p1;
  unsigned n = unsigned(1.5+vcl_max(vcl_fabs(dp.x()),vcl_fabs(dp.y())));
  dp/=n;
  unsigned ni=image.ni(), nj=image.nj();
  const vcl_ptrdiff_t rstep = 0;
  const vcl_ptrdiff_t gstep = image.planestep();
  const vcl_ptrdiff_t bstep =2*gstep;
  for (unsigned i=0;i<=n;++i,p1+=dp)
  {
    unsigned pi=unsigned(p1.x()+0.5); if (pi>=ni) continue;
    unsigned pj=unsigned(p1.y()+0.5); if (pj>=nj) continue;
    T* im = &image(pi,pj);
    im[rstep]=r;
    im[gstep]=g;
    im[bstep]=b;
  }
}


#endif // mbl_draw_line_h_


