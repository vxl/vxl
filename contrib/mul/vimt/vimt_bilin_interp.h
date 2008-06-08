// This is mul/vimt/vimt_bilin_interp.h
#ifndef vimt_bilin_interp_h_
#define vimt_bilin_interp_h_
//:
//  \file
//  \brief Bilinear interpolation functions for 2D images
//  \author Tim Cootes

#include <vil/vil_bilin_interp.h>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_point_2d.h>

//: Compute bilinear interpolation at (x,y) in world coordinates, no bound checks
//  Interpolates given plane of image.image() at image.world2im(p)
template<class T>
inline double vimt_bilin_interp_safe(const vimt_image_2d_of<T>& image,
                                     const vgl_point_2d<double>& p,
                                     int plane=0)
{
  vgl_point_2d<double> im_p = image.world2im()(p);
  const vil_image_view<T>& im = image.image();
  return vil_bilin_interp_safe(im_p.x(),im_p.y(),
                               im.top_left_ptr()+plane*im.planestep(),
                               im.ni(),im.nj(),  im.istep(),im.jstep());
}

#endif // vimt_bilin_interp_h_
