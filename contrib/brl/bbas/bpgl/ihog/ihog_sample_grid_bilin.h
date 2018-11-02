#ifndef ihog_sample_grid_bilin_h_
#define ihog_sample_grid_bilin_h_
//:
// \file
// \brief Grid sampling functions for 2D images
// \author Tim Cootes
// \verbatim
//  Modifications
//   migrated to ihog - G. Tunali  Aug.2010
// \endverbatim
#include <vil/vil_resample_bilin.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include "ihog_transform_2d.h"
#include "ihog_image.h"
#include "ihog_utils.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Sample grid from image, using bilinear interpolation
//  Grid points are p+i.u+j.v where i=[0..n1-1], j=[0..n2-1]
//  Vector vec is resized to n1*n2*np elements, where np=image.nplanes().
//  vec[0]..vec[np-1] are the values from point p
//  Samples are taken along direction v first, then along u.
//  Points outside image return zero.
// \relatesalso ihog_image_2d
void ihog_sample_grid_bilin(vnl_vector<double>& vec,
                            const ihog_image<float>& image,
                            const vgl_point_2d<double>& p,
                            const vgl_vector_2d<double>& u,
                            const vgl_vector_2d<double>& v,
                            int n1, int n2);

//: Sample grid of points in one image and place in another, using bilinear interpolation.
//  dest_image(i,j,p) is sampled from the src_image at
//  p+i.u+j.v, where i=[0..n1-1], j=[0..n2-1] in world co-ordinates.
//
//  dest_image resized to (n1,n2,src_image.nplanes())
//
//  dest_image.world2im() set up so that the world co-ordinates in src and dest match
//
//  Points outside image return zero.
inline void ihog_resample_bilin(const ihog_image<float>& src_image,
                                ihog_image<float>& dest_image,
                                const vgl_point_2d<double>& p,
                                const vgl_vector_2d<double>& u,
                                const vgl_vector_2d<double>& v,
                                int n1, int n2)
{
  // Not implemented for projective yet
  assert(src_image.world2im().form()!=ihog_transform_2d::Projective);

  const ihog_transform_2d& s_w2i = src_image.world2im();
  vgl_point_2d<double> im_p = s_w2i(p);
  vgl_vector_2d<double> im_u = s_w2i.delta(p, u);
  vgl_vector_2d<double> im_v = s_w2i.delta(p, v);

  vil_resample_bilin(src_image.image(),dest_image.image(),
                     im_p.x(),im_p.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(), n1,n2);

  // Point (i,j) in dest corresponds to p+i.u+j.v,
  // an affine transformation for image to world
  ihog_transform_2d d_i2w;
  d_i2w.set_affine(p,u,v);
  dest_image.set_world2im(d_i2w.inverse());
}
//: transform/resample image to a size that contains the full result
inline void ihog_resample_bilin(const ihog_image<float>& src_image,
                                ihog_image<float>& dest_image,
                                const vgl_point_2d<double>& p,
                                const vgl_vector_2d<double>& u,
                                const vgl_vector_2d<double>& v)
{
  // Not implemented for projective yet
  assert(src_image.world2im().form()!=ihog_transform_2d::Projective);

  const ihog_transform_2d& s_w2i = src_image.world2im();
  vgl_point_2d<double> im_p = s_w2i(p);
  vgl_vector_2d<double> im_u = s_w2i.delta(p, u);
  vgl_vector_2d<double> im_v = s_w2i.delta(p, v);
  ihog_transform_2d t, t_comp;
  t.set_affine(im_p, im_u, im_v);
  unsigned source_ni=src_image.image().ni(), source_nj=src_image.image().nj();
  unsigned dest_ni, dest_nj;
  ihog_utils::image_bounds(source_ni, source_nj, t, dest_ni, dest_nj, t_comp);
  //extract affine parameters
  vgl_point_2d<double> p_comp = t_comp.origin();
  vgl_vector_2d<double> u_comp =
    t_comp.delta(p_comp, vgl_vector_2d<double>(1,0));
  vgl_vector_2d<double> v_comp =
    t_comp.delta(p_comp, vgl_vector_2d<double>(0,1));

  //resample from source to destination
  vil_resample_bilin(src_image.image(),dest_image.image(),
                     p_comp.x(),p_comp.y(),
                     u_comp.x(),u_comp.y(),
                     v_comp.x(),v_comp.y(), dest_ni,dest_nj);

  dest_image.set_world2im(t_comp.inverse());
}
//: specify a transform for resampling, map to a specified image size
inline void ihog_resample_bilin(const ihog_image<float>& src_image,
                                ihog_image<float>& dest_image,
                                ihog_transform_2d const& tr,
                                int n1, int n2){
  vgl_point_2d<double> p = tr.origin();
  vgl_vector_2d<double> du = tr.delta(p, vgl_vector_2d<double>(1,0));
  vgl_vector_2d<double> dv = tr.delta(p, vgl_vector_2d<double>(0,1));
  ihog_resample_bilin(src_image,dest_image, p, du, dv, n1, n2);
}
//: specify a transform for resampling, determine size to contain result
inline void ihog_resample_bilin(const ihog_image<float>& src_image,
                                ihog_image<float>& dest_image,
                                ihog_transform_2d const& tr){
  vgl_point_2d<double> p = tr.origin();
  vgl_vector_2d<double> du = tr.delta(p, vgl_vector_2d<double>(1,0));
  vgl_vector_2d<double> dv = tr.delta(p, vgl_vector_2d<double>(0,1));
  ihog_resample_bilin(src_image,dest_image, p, du, dv);
}

#endif // ihog_sample_grid_bilin_h_
