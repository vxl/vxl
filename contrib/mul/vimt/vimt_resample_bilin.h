#ifndef vimt_resample_bilin_h_
#define vimt_resample_bilin_h_
//:
// \file
// \brief Sample grid of points in one image and place in another
// \author Tim Cootes

#include <vcl_cassert.h>
#include <vil/vil_resample_bilin.h>
#include <vil/algo/vil_gauss_reduce.h>
#include <vimt/vimt_image_2d_of.h>

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
inline void vimt_resample_bilin(
  const vimt_image_2d_of<sType>& src_image,
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
  vgl_vector_2d<double> im_u = s_w2i.delta(p, u);
  vgl_vector_2d<double> im_v = s_w2i.delta(p, v);

  vil_resample_bilin(src_image.image(),dest_image.image(),
                      im_p.x(),im_p.y(),  im_u.x(),im_u.y(),
                      im_v.x(),im_v.y(), n1,n2);

  // Point (i,j) in dest corresponds to p+i.u+j.v,
  // an affine transformation for image to world
  vimt_transform_2d d_i2w;
  d_i2w.set_affine(p,u,v);
  dest_image.set_world2im(d_i2w.inverse());
}


//: Resample an image using appropriate smoothing if the resolution changes significantly.
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
inline void vimt_resample_bilin_smoothing(
  const vimt_image_2d_of<sType>& src_image,
  vimt_image_2d_of<dType>& dest_image,
  const vgl_point_2d<double>& p,
  const vgl_vector_2d<double>& u,
  const vgl_vector_2d<double>& v,
  int n1, int n2)
{
  // Not implemented for projective yet
  assert(src_image.world2im().form()!=vimt_transform_2d::Projective);

  vimt_transform_2d scaling;
  scaling.set_zoom_only(0.5,0,0);

  vimt_image_2d_of<sType> im = src_image;
  vgl_vector_2d<double> im_d = im.world2im().delta(p, u) + im.world2im().delta(p, v);

  // If step length (in pixels) >> 1, smooth and reduce image x2.
  // Don't use strict Nyqusit limit.
  // Since we are just using factor 2 smoothing and reduction,
  // we have to tradeoff aliasing with information loss.
  while (im_d.length() > 1.33*1.414 && im.image().ni() > 5 && im.image().nj() > 5)
  {
    vimt_image_2d_of<sType> dest;
    vil_image_view<sType> work;
    vil_gauss_reduce(im.image(), dest.image(), work);
    
    dest.set_world2im(scaling * im.world2im());
    
    // re-establish loop invariant
    im = dest;
    im_d = im.world2im().delta(p, u) + im.world2im().delta(p, v);
  }

  vimt_resample_bilin(im, dest_image, p, u, v, n1, n2);
}

#endif // vimt_resample_bilin_h_
