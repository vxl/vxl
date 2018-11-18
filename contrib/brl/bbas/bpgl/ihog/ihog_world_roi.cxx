// This is bbas/bpgl/ihog/ihog_world_roi.cxx
#include "ihog_world_roi.h"
//:
// \file

#include "ihog_sample_grid_bilin.h"
#include <vil/vil_resample_bilin.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor
ihog_world_roi::ihog_world_roi( unsigned int n_u,  unsigned int n_v,
                                const vgl_point_2d< double > &  p0,
                                const vgl_vector_2d< double > &  u,
                                const vgl_vector_2d< double > &  v )
 : n_u_(n_u), n_v_(n_v), p0_(p0), u_(u), v_(v)
{
}


//: Sample the image in the ROI and return a vector of values
vnl_vector<double>
ihog_world_roi::sample( const ihog_image<float>& image) const
{
  vnl_vector<double> result;
  ihog_sample_grid_bilin(result, image, p0_, u_, v_, n_u_, n_v_);
  return result;
}


//: Create a vector of weights for the sample of \p image
//  Weights are based on distance from the image boundaries
vnl_vector<double>
ihog_world_roi::sample_weights( const ihog_image<float>& image) const
{
  unsigned int ni = image.image().ni();
  unsigned int nj = image.image().nj();
  vnl_vector<double> result(ni*nj);
  double *vec_data = result.data_block();
  const ihog_transform_2d& w2i = image.world2im();
  vgl_point_2d<double> p1=p0_;
  for (unsigned i=0; i<n_u_; ++i, p1+=u_){
    vgl_point_2d<double> p=p1;  // Start of j-th row
    for (unsigned j=0; j<n_v_; ++j,p+=v_, ++vec_data){
      vgl_point_2d<double> im_p = w2i(p);
      if (im_p.x() < 0 || im_p.x() > ni-1 || im_p.y() < 0 || im_p.y() > nj-1)
        *vec_data = 0.0;
      else{
        double xdist = (im_p.x() > (ni-1)/2.0)?((ni-1)-im_p.x()):(im_p.x());
        double ydist = (im_p.y() > (nj-1)/2.0)?((nj-1)-im_p.y()):(im_p.y());
        double dist = (xdist < ydist)?xdist:ydist;
        *vec_data = (dist > 3.0)?(1.0):(dist/3.0);
      }
    }
  }
  return result;
}


//: Sample the image in the ROI and return an image
vil_image_view<float>
ihog_world_roi::resample( const ihog_image<float>& image) const
{
  ihog_image<float> result;
  //vimt_resample_bilin(image, result, p0_, u_, v_, n_u_, n_v_);

  // Not implemented for projective yet
  assert(image.world2im().form()!=ihog_transform_2d::Projective);

  vgl_point_2d<double> im_p = image.world2im()(p0_);
  vgl_vector_2d<double> im_u = image.world2im().delta(p0_, u_);
  vgl_vector_2d<double> im_v = image.world2im().delta(p0_, v_);

  vil_resample_bilin(image.image(),result.image(),
                     im_p.x(),im_p.y(),  im_u.x(),im_u.y(),
                     im_v.x(),im_v.y(), n_u_,n_v_);

  // Point (i,j) in dest corresponds to p+i.u+j.v,
  // an affine transformation for image to world
  ihog_transform_2d d_i2w;
  d_i2w.set_affine(p0_,u_,v_);
  result.set_world2im(d_i2w.inverse());
  return result.image();
}
