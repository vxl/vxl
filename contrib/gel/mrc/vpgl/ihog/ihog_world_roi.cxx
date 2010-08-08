// This is vpgl/ihog/ihog_world_roi.cxx
//:
// \file

#include "ihog_world_roi.h"

#include <vimt/vimt_sample_grid_bilin.h>
#include <vimt/vimt_resample_bilin.h>


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
ihog_world_roi::sample( const vimt_image_2d_of<float>& image) const
{
  vnl_vector<double> result;
  vimt_sample_grid_bilin(result, image, p0_, u_, v_, n_u_, n_v_);
  return result;
}


//: Create a vector of weights for the sample of \p image
//  weights are based on distance from the image boundaries
vnl_vector<double>
ihog_world_roi::sample_weights( const vimt_image_2d_of<float>& image) const
{
  unsigned int ni = image.image().ni();
  unsigned int nj = image.image().nj();
  vnl_vector<double> result(ni*nj);
  double *vec_data = result.data_block();
  const vimt_transform_2d& w2i = image.world2im();
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
ihog_world_roi::resample( const vimt_image_2d_of<float>& image) const
{
  vimt_image_2d_of<float> result;
  vimt_resample_bilin(image, result, p0_, u_, v_, n_u_, n_v_);
  return result.image();
}



