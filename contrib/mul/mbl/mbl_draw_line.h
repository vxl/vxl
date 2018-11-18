#ifndef mbl_draw_line_h_
#define mbl_draw_line_h_
//:
// \file
// \brief Functions to draw shapes into an image (may get moved to vil in time)
// \author Tim Cootes

#include <iostream>
#include <algorithm>
#include <cmath>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Draws value along line between p1 and p2
//  Effective, but not terribly efficient.
template<class T>
inline void mbl_draw_line(vil_image_view<T>& image,
                          vgl_point_2d<double> p1,
                          vgl_point_2d<double> p2,
                          T value, unsigned width=1)
{
  vgl_vector_2d<double> dp = p2-p1;
  unsigned n = unsigned(1.5+std::max(std::fabs(dp.x()),std::fabs(dp.y())));
  dp/=n;
  unsigned ni=image.ni(), nj=image.nj();
  if (width==1)
  {
    for (unsigned i=0;i<=n;++i,p1+=dp)
    {
      unsigned pi=unsigned(p1.x()+0.5); if (pi>=ni) continue;
      unsigned pj=unsigned(p1.y()+0.5); if (pj>=nj) continue;
      image(pi,pj)=value;
    }
  }
  else
  {
    double sw=double(unsigned(width/2));
    vgl_vector_2d<double> normal(-dp.y(),dp.x());
    normal=normalized(normal);
    for (unsigned i=0;i<=n;++i,p1+=dp)
    {
      vgl_point_2d<double> p3=p1-sw*normal;
      for ( unsigned j=0;j<width;++j,p3+=normal)
      {
        unsigned pi=unsigned(p3.x()+0.5); if (pi>=ni) continue;
        unsigned pj=unsigned(p3.y()+0.5); if (pj>=nj) continue;
        image(pi,pj)=value;
      }
    }
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
  unsigned n = unsigned(1.5+std::max(std::fabs(dp.x()),std::fabs(dp.y())));
  dp/=n;
  unsigned ni=image.ni(), nj=image.nj();
  constexpr std::ptrdiff_t rstep = 0;
  const std::ptrdiff_t gstep = image.planestep();
  const std::ptrdiff_t bstep =2*gstep;
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
