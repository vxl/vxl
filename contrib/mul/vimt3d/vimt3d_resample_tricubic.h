// This is mul/vimt3d/vimt3d_resample_tricubic.h
#ifndef vimt3d_resample_tricubic_h_
#define vimt3d_resample_tricubic_h_
//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Gwenael Guillard

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>
#include <vil3d/vil3d_resample_tricubic.h>
#include <vimt3d/vimt3d_image_3d_of.h>

//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at p+i.u+j.v+k.w,
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1] in world co-ordinates.
//
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//
//  dest_image.world2im() set up so that the world co-ordinates in src and dest match
//
//  Points outside image return zero or \a outval
template <class sType, class dType>
inline void vimt3d_resample_tricubic(
  const vimt3d_image_3d_of<sType>& src_image,
  vimt3d_image_3d_of<dType>& dest_image,
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  int n1, int n2, int n3,
  dType outval=0)
{
  const vimt3d_transform_3d& s_w2i = src_image.world2im();
  vgl_point_3d<double> im_p = s_w2i(p);
  vgl_vector_3d<double> im_u = s_w2i.delta(p, u);
  vgl_vector_3d<double> im_v = s_w2i.delta(p, v);
  vgl_vector_3d<double> im_w = s_w2i.delta(p, w);

  vil3d_resample_tricubic(src_image.image(),dest_image.image(),
                           im_p.x(), im_p.y(), im_p.z(),
                           im_u.x(), im_u.y(), im_u.z(),
                           im_v.x(), im_v.y(), im_v.z(),
                           im_w.x(), im_w.y(), im_w.z(),
                           n1, n2, n3,
                           outval);

  // Point (i,j,k) in dest corresponds to p+i.u+j.v+k.w,
  // an affine transformation for image to world
  vimt3d_transform_3d d_i2w;
  d_i2w.set_affine(p,u,v,w);
  d_i2w.simplify();
  dest_image.set_world2im(d_i2w.inverse());
}


//: Sample grid of points in one image and place in another, using tricubic interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  p+i.u+j.v+k.w, where i=[0..nk-1], j=[0..nj-1], k=[0..nk-1] in world co-ordinates.
//
//  dest_image resized to (ni,nj,nk,src_image.nplanes())
//
//  dest_image.world2im() set up so that the world co-ordinates in src and dest match
//
//  Points outside image return the value of the nearest valid pixel.
// \relatesalso vimt3d_image_3d_of
template <class sType, class dType>
inline void vimt3d_resample_tricubic_edge_extend(
  const vimt3d_image_3d_of<sType>& src_image,
  vimt3d_image_3d_of<dType>& dest_image,
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  int ni, int nj, int nk)
{
  const vimt3d_transform_3d& s_w2i = src_image.world2im();
  vgl_point_3d<double> im_p = s_w2i(p);
  vgl_vector_3d<double> im_u = s_w2i.delta(p, u);
  vgl_vector_3d<double> im_v = s_w2i.delta(p, v);
  vgl_vector_3d<double> im_w = s_w2i.delta(p, w);

  vil3d_resample_tricubic_edge_extend(src_image.image(),dest_image.image(),
    im_p.x(),im_p.y(),im_p.z(), im_u.x(),im_u.y(),im_u.z(),
    im_v.x(),im_v.y(),im_v.z(), im_w.x(),im_w.y(),im_w.z(), ni,nj,nk);

  // Point (i,j,k) in dest corresponds to p+i.u+j.v+k.w,
  // an affine transformation for image to world
  vimt3d_transform_3d d_i2w;
  d_i2w.set_affine(p,u,v,w);
  d_i2w.simplify();
  dest_image.set_world2im(d_i2w.inverse());
}


#endif // vimt3d_resample_tricubic_h_
