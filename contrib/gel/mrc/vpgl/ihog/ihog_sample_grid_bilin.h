#ifndef ihog_sample_grid_bilin_h_
#define ihog_sample_grid_bilin_h_
//:
// \file
// \brief Grid sampling functions for 2D images
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vil/vil_resample_bilin.h>
#include <vnl/vnl_fwd.h>
#include <vgl/vgl_fwd.h>
#include "ihog_transform_2d.h"
#include "ihog_image.h"
#include <vcl_cassert.h>

//: Sample grid from image, using bilinear interpolation
//  Grid points are p+i.u+j.v where i=[0..n1-1], j=[0..n2-1]
//  Vector vec is resized to n1*n2*np elements, where np=image.nplanes().
//  vec[0]..vec[np-1] are the values from point p
//  Samples are taken along direction v first, then along u.
//  Points outside image return zero.
// \relatesalso ihog_image_2d_of
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
// \relatesalso vimt_image_view
inline void ihog_resample_bilin(
  const ihog_image<float>& src_image,
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

#endif // ihog_sample_grid_bilin_h_
