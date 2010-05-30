// This is mul/vimt3d/vimt3d_resample_trilinear.h
#ifndef vimt3d_resample_trilinear_h_
#define vimt3d_resample_trilinear_h_
//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/algo/vil3d_gauss_reduce.h>
#include <vil3d/vil3d_resample_trilinear.h>
#include <vimt3d/vimt3d_image_3d_of.h>

//: Resample a 3D image by a factor of 2 in each dimension.
// \p dst_image has size 2*src.image().n?()-1 in each direction.
// Transform is modified by an appropriate scaling of 0.5
// Interpolated values are truncated when the type T is smaller than double.
// \sa vil3d_resample_trilinear_scale_2()
template <class T>
void vimt3d_resample_trilinear_scale_2(
  const vimt3d_image_3d_of<T>& src,
  vimt3d_image_3d_of<T>& dst)
{
  vil3d_resample_trilinear_scale_2(src.image(), dst.image());

  vimt3d_transform_3d scaling;
  scaling.set_zoom_only(2.0, 2.0, 2.0, 0.0, 0.0, 0.0);
  dst.set_world2im(scaling * src.world2im());
}


//: Sample grid of points in one image and place in another, using trilinear interpolation.
//  dest_image(i,j,k,p) is sampled from the src_image at p+i.u+j.v+k.w,
//  where i=[0..n1-1], j=[0..n2-1], k=[0..n3-1] in world co-ordinates.
//
//  dest_image resized to (n1,n2,n3,src_image.nplanes())
//
//  dest_image.world2im() set up so that the world co-ordinates in src and dest match
//
//  Points outside image return zero or \a outval
template <class sType, class dType>
inline void vimt3d_resample_trilinear(
  const vimt3d_image_3d_of<sType>& src_image,
  vimt3d_image_3d_of<dType>& dest_image,
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  int n1, int n2, int n3,
  dType outval=0, double edge_tol=0)
{
  const vimt3d_transform_3d& s_w2i = src_image.world2im();
  vgl_point_3d<double> im_p = s_w2i(p);
  vgl_vector_3d<double> im_u = s_w2i.delta(p, u);
  vgl_vector_3d<double> im_v = s_w2i.delta(p, v);
  vgl_vector_3d<double> im_w = s_w2i.delta(p, w);

  vil3d_resample_trilinear(src_image.image(),dest_image.image(),
                           im_p.x(), im_p.y(), im_p.z(),
                           im_u.x(), im_u.y(), im_u.z(),
                           im_v.x(), im_v.y(), im_v.z(),
                           im_w.x(), im_w.y(), im_w.z(),
                           n1, n2, n3,
                           outval, edge_tol);

  // Point (i,j,k) in dest corresponds to p+i.u+j.v+k.w,
  // an affine transformation for image to world
  vimt3d_transform_3d d_i2w;
  d_i2w.set_affine(p,u,v,w);
  d_i2w.simplify();
  dest_image.set_world2im(d_i2w.inverse());
}


//: Sample grid of points in one image and place in another, using trilinear interpolation.
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
inline void vimt3d_resample_trilin_edge_extend(
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

  vil3d_resample_trilinear_edge_extend(src_image.image(),dest_image.image(),
                                       im_p.x(),im_p.y(),im_p.z(), im_u.x(),im_u.y(),im_u.z(),
                                       im_v.x(),im_v.y(),im_v.z(), im_w.x(),im_w.y(),im_w.z(), ni,nj,nk);

  // Point (i,j,k) in dest corresponds to p+i.u+j.v+k.w,
  // an affine transformation for image to world
  vimt3d_transform_3d d_i2w;
  d_i2w.set_affine(p,u,v,w);
  d_i2w.simplify();
  dest_image.set_world2im(d_i2w.inverse());
}


//: Resample an image using appropriate smoothing if the resolution changes significantly.
//  dest_image(i,j,k,p) is sampled from the src_image at
//  p+i.u+j.v+k.w, where i=[0..ni-1], j=[0..nj-1], k=[0..nk-1] in world co-ordinates.
//
//  dest_image resized to (ni,nj,nk,src_image.nplanes())
//
//  dest_image.world2im() set up so that the world co-ordinates in src and dest match
//
//  Points outside image return the value of the nearest valid pixel.
// \relatesalso vimt3d_image_3d_of
template <class sType, class dType>
inline void vimt3d_resample_trilin_smoothing_edge_extend(
  const vimt3d_image_3d_of<sType>& src_image,
  vimt3d_image_3d_of<dType>& dest_image,
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  int ni, int nj, int nk)
{
  vimt3d_transform_3d scaling;
  scaling.set_zoom_only(0.5,0,0,0);

  vimt3d_image_3d_of<sType> im = src_image;
  vgl_vector_3d<double> im_d = im.world2im().delta(p, u) + im.world2im().delta(p, v)
    + im.world2im().delta(p, w);

  // If step length (in pixels) >> 1, smooth and reduce image x2.
  // Don't use strict Nyqusit limit.
  // Since we are just using factor 2 smoothing and reduction,
  // we have to tradeoff aliasing with information loss.
  while (im_d.length() > 1.33*1.732 && im.image().ni() > 5 && im.image().nj() > 5 && im.image().nk() > 5)
  {
    vimt3d_image_3d_of<sType> dest;
    vil3d_image_view<sType> work1, work2;
    vil3d_gauss_reduce(im.image(), dest.image(), work1, work2);

    dest.set_world2im(scaling * im.world2im());

    // re-establish loop invariant
    im = dest;
    im_d = im.world2im().delta(p, u) + im.world2im().delta(p, v) + im.world2im().delta(p, w);
  }

  vimt3d_resample_trilin_edge_extend(im, dest_image, p, u, v, w, ni, nj, nk);
}

//: Resample src, using the grid defined by dest.
//  Smooths appropriately if the resolution changes significantly.
//  dest(i,j,k,p) is sampled from src at the wc grid defined by
//  the world co-ords of the pixel centres in dest.
//
//  dest is not resized, nor has its world2im transform modified.
//
//  Points outside src return the value of the nearest valid pixel.
// \relatesalso vimt3d_image_3d_of
template <class sType, class dType>
inline void vimt3d_resample_trilin_smoothing_edge_extend(
  const vimt3d_image_3d_of<sType>& src,
  vimt3d_image_3d_of<dType>& dest)
{
  vimt3d_transform_3d orig_w2i = dest.world2im();
  vimt3d_transform_3d i2w = orig_w2i.inverse();

  vimt3d_resample_trilin_smoothing_edge_extend(
    src, dest,
    i2w.origin(),
    i2w.delta(vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1,0,0)),
    i2w.delta(vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(0,1,0)),
    i2w.delta(vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(0,0,1)),
    dest.image().ni(), dest.image().nj(), dest.image().nk() );

  dest.world2im() = orig_w2i;
}


#endif // vimt3d_resample_trilinear_h_
