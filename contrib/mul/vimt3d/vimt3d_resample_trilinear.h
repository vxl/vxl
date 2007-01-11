// This is mul/vimt3d/vimt3d_resample_trilinear.h
#ifndef vimt3d_resample_trilinear_h_
#define vimt3d_resample_trilinear_h_

//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza

#include <vil3d/vil3d_image_view.h>
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
//  Points outside image return zero.
template <class sType, class dType>
inline void vimt3d_resample_trilinear(
  const vimt3d_image_3d_of<sType>& src_image,
  vimt3d_image_3d_of<dType>& dest_image,
  const vgl_point_3d<double>& p,
  const vgl_vector_3d<double>& u,
  const vgl_vector_3d<double>& v,
  const vgl_vector_3d<double>& w,
  int n1, int n2, int n3)
{
  // Only implemented for transformations up to Affine
//   assert(src_image.world2im().form()!=vimt3d_transform_3d::Projective);

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
    n1, n2, n3);

  // Point (i,j,k) in dest corresponds to p+i.u+j.v+k.w,
  // an affine transformation for image to world
  vimt3d_transform_3d d_i2w;
  d_i2w.set_affine(p,u,v,w);
  dest_image.set_world2im(d_i2w.inverse());
}


#endif // vimt3d_resample_trilinear_h_
