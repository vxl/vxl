// This is mul/vimt3d/vimt3d_trilin_interp.h
#ifndef vimt3d_trilin_interp_h_
#define vimt3d_trilin_interp_h_
//:
// \file
// \brief Bilinear interpolation functions for 2D images
// \author Tim Cootes

#include <vimt3d/vimt3d_image_3d_of.h>
#include <vgl/vgl_point_3d.h>
#include <vil3d/vil3d_trilin_interp.h>
#include <vil3d/vil3d_image_view.h>

//: Compute trilinear interpolation at p(x,y,z) in world coordinates, with bound checks.
//  Interpolates given plane of image.image() at image.world2im(p).
//  If p is outside interpolatable image region, returns zero or \a outval
template<class T>
inline double vimt3d_trilin_interp_safe(const vimt3d_image_3d_of<T>& image,
                                        const vgl_point_3d<double>& p,
                                        unsigned plane=0,
                                        T outval=0)
{
  vgl_point_3d<double> im_p = image.world2im()(p);
  const vil3d_image_view<T>& im = image.image();
  return vil3d_trilin_interp_safe(im_p.x(),im_p.y(),im_p.z(),
                                  im.origin_ptr()+plane*im.planestep(),
                                  im.ni(),im.nj(),im.nk(),
                                  im.istep(),im.jstep(),im.kstep(),
                                  outval);
}

//: Compute trilinear interpolation at p(x,y,z) in world coordinates, no bound checks.
//  Interpolates given plane of image.image() at image.world2im(p).
template<class T>
inline double vimt3d_trilin_interp_raw(const vimt3d_image_3d_of<T>& image,
                                       const vgl_point_3d<double>& p,
                                       unsigned plane=0)
{
  vgl_point_3d<double> im_p = image.world2im()(p);
  const vil3d_image_view<T>& im = image.image();
  return vil3d_trilin_interp_raw(im_p.x(),im_p.y(),im_p.z(),
                                 im.origin_ptr()+plane*im.planestep(),
                                 im.istep(),im.jstep(),im.kstep());
}

//: Compute trilinear interpolation at p(x,y,z) in world coordinates, using the nearest valid value if out of bounds.
//  Interpolates given plane of image.image() at image.world2im(p).
//  If p is outside safe interpolatable image region, nearest pixel value is returned.
template<class T>
inline double vimt3d_trilin_interp_safe_extend(const vimt3d_image_3d_of<T>& image,
                                               const vgl_point_3d<double>& p,
                                               unsigned plane=0)
{
  vgl_point_3d<double> im_p = image.world2im()(p);
  const vil3d_image_view<T>& im = image.image();
  return vil3d_trilin_interp_safe_extend(im_p.x(),im_p.y(),im_p.z(),
                                         im.origin_ptr()+plane*im.planestep(),
                                         im.ni(),im.nj(),im.nk(),
                                         im.istep(),im.jstep(),im.kstep());
}

#endif // vimt3d_trilin_interp_h_
