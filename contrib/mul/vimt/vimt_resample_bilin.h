#ifndef vimt_resample_bilin_h_
#define vimt_resample_bilin_h_
//:
// \file
// \brief Sample grid of points in one image and place in another
// \author Tim Cootes

#include <vil2/vil2_resample_bilin.h>
#include <vimt/vimt_image_2d_of.h>
#include <vcl_cassert.h>

//: Sample grid of points in one image and place in another, using bilinear interpolation.
//  dest_image(i,j,p) is sampled from the src_image at
//  p+i.u+j.v, where i=[0..n1-1], j=[0..n2-1] in world co-ordinates.
//
//  dest_image resized to (n1,n2,src_image.nplanes())
//
//  dest_image.world2im() set up so that the world co-ordinates in src and dest match
//
//  Points outside image return zero.
// \relates vimt_image_view
template <class sType, class dType>
inline
void vimt_resample_bilin(const vimt_image_2d_of<sType>& src_image,
                         vimt_image_2d_of<dType>& dest_image,
                         const vgl_point_2d<double>& p,
                         const vgl_vector_2d<double>& u,
                         const vgl_vector_2d<double>& v,
                         int n1, int n2)
{
  // Not implemented for projective yet
  assert(src_image.world2im().form()!=vimt_transform_2d::Projective);

  const vimt_transform_2d& s_w2i = src_image.world2im();
  vgl_point_2d<double> im_p = s_w2i(p);
  vgl_vector_2d<double> im_u = s_w2i(p+u)-im_p;
  vgl_vector_2d<double> im_v = s_w2i(p+v)-im_p;

  vil2_resample_bilin(src_image.image(),dest_image.image(),
                      im_p.x(),im_p.y(),  im_u.x(),im_u.y(),
                      im_v.x(),im_v.y(), n1,n2);

  // Point (i,j) in dest corresponds to p+i.u+j.v,
  // an affine transformation for image to world
  vimt_transform_2d d_i2w;
  d_i2w.set_affine(p,u,v);
  dest_image.set_world2im(d_i2w.inverse());
}

#endif // vimt_resample_bilin_h_
