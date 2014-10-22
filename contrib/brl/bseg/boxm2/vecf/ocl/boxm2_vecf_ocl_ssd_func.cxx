#include "boxm2_vecf_ocl_ssd_func.h"

boxm2_vecf_ocl_ssd_func::boxm2_vecf_ocl_ssd_func(unsigned nx, unsigned ni, unsigned nj, boxm2_vecf_ocl_transform_scene_sptr tscn) :
  vnl_least_squares_function(nx, ni*nj, vnl_least_squares_function::no_gradient), tscn_(tscn)
{
}

void boxm2_vecf_ocl_ssd_func::f(vnl_vector<double> const& x, vnl_vector<double>& fx){
  vil_image_view<float> diff = this->diff_img(x);
  unsigned k = 0;
  for(unsigned j = 0; j<nj_; ++j)
    for(unsigned i = 0; i<ni_; ++i, ++k)
      fx[k]=diff(i,j);
}
vil_image_view<float> boxm2_vecf_ocl_ssd_func::diff_img(vnl_vector<double> const& x) const{
  vil_image_view<float> ret(ni_, nj_);
  vgl_vector_3d<double> scale;
  this->x_to_scale(x, scale);
  tscn_->transform_1_blk_interp(rot_, trans_, scale, false);
  vil_image_view<float> exp, vis;
  tscn_->render_scene_appearance(ref_cam_, exp, vis, ni_, nj_, false);
  for(unsigned j = 0; j<nj_; ++j)
    for(unsigned i = 0; i<ni_; ++i){
      float vi= vis(i,j), ex = exp(i,j), re = ref_img_(i,j);
      float d = vcl_fabs((ex-re)*vi);
      ret(i,j)=d;
    }
  return ret;
}
void boxm2_vecf_ocl_ssd_func::finish(){
  vil_image_view<float> exp, vis;
  vgl_vector_3d<double> scale(1.0, 1.0, 1.0);
  tscn_->render_scene_appearance(ref_cam_, exp, vis, ni_, nj_, true);
  tscn_->transform_1_blk_interp(rot_, trans_, scale, true);
}
