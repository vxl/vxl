#include "boxm2_vecf_ocl_ssd_func.h"

boxm2_vecf_ocl_ssd_func::boxm2_vecf_ocl_ssd_func(unsigned nx, unsigned ni, unsigned nj, const boxm2_vecf_ocl_transform_scene_sptr& tscn,const boxm2_scene_sptr& target_scene,const boxm2_opencl_cache_sptr& ocl_cache):
  ni_(ni), nj_(nj),
  vnl_least_squares_function(nx, ni*nj, vnl_least_squares_function::no_gradient), tscn_(tscn),renderer_(target_scene,ocl_cache),depth_renderer_(target_scene , ocl_cache)
{
}

void boxm2_vecf_ocl_ssd_func::f(vnl_vector<double> const& x, vnl_vector<double>& fx){
  vil_image_view<float> diff = this->diff_img(x);
  unsigned k = 0;
  for(unsigned j = 0; j<nj_; ++j)
    for(unsigned i = 0; i<ni_; ++i, ++k)
      fx[k]=diff(i,j);
}

vil_image_view<float> boxm2_vecf_ocl_ssd_func::diff_img(vnl_vector<double> const& x) {
  vil_image_view<float> ret(ni_, nj_);
  vgl_vector_3d<double> scale;
  this->x_to_scale(x, scale);
  tscn_->transform_1_blk_interp(rot_, trans_, scale, false);
  vil_image_view<float> exp, vis;

  this->render_scene_appearance(ref_cam_, exp, vis, ni_, nj_);
  for(unsigned j = 0; j<nj_; ++j)
    for(unsigned i = 0; i<ni_; ++i){
      float vi= vis(i,j), ex = exp(i,j), re = ref_img_(i,j);
      float d = std::fabs((ex-re)*vi);
      ret(i,j)=d;
    }
  return ret;
}
void boxm2_vecf_ocl_ssd_func::finish(){
  vil_image_view<float> exp, vis;
  vgl_vector_3d<double> scale(1.0, 1.0, 1.0);
  this->render_scene_appearance(ref_cam_, exp, vis, ni_, nj_);
  tscn_->transform_1_blk_interp(rot_, trans_, scale, true);
}

  //:render the current state of the target scene leaving scene GPU buffers in place
  // thus rendering can be faster since block buffer transfers are not needed
  //:render the depth of the current state of the target scene leaving scene GPU buffers in place


bool boxm2_vecf_ocl_ssd_func::render_scene_appearance(vpgl_camera_double_sptr const& cam,
                        vil_image_view<float>& expected_img, vil_image_view<float>& vis_img,
                        unsigned ni, unsigned nj) {

  bool status = renderer_.render(cam, ni, nj);
  renderer_.get_last_vis(vis_img);
  renderer_.get_last_rendered(expected_img);

  return status;
}

bool boxm2_vecf_ocl_ssd_func::render_scene_depth(vpgl_camera_double_sptr const & cam,
                                                        vil_image_view<float>& expected_depth,
                                                        vil_image_view<float>& vis_img,
                                                        unsigned ni, unsigned nj)
{
  bool status = depth_renderer_.render(cam, ni, nj);
  depth_renderer_.get_last_vis(vis_img);
  depth_renderer_.get_last_rendered(expected_depth);
  return status;
}
