#ifndef boxm_render_expected_depth_functor_h
#define boxm_render_expected_depth_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_scene.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_rt_sample.h>

#include <vil/vil_transform.h>
#include <vpgl/vpgl_camera.h>

template <boxm_apm_type APM, class T_aux>
class boxm_render_expected_depth_functor
{
 public:
  //: "default" constructor
  boxm_render_expected_depth_functor(vil_image_view<float> &expected,
                                     vil_image_view<float> &mask_vis,
                                     vgl_point_3d<double> cam_center,
                                     unsigned int ni,unsigned nj,
                                     bool scene_read_only=false,
                                     bool is_aux=true)
  : expected_(expected), vis_img_(mask_vis), alpha_integral_(ni,nj,1), center_(cam_center)
  {
    alpha_integral_.fill(0.0f);
    expected_.fill(0.0f);
    vis_img_.fill(1.0f);
    scene_read_only_=scene_read_only;
    is_aux_=is_aux;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> &cell_value, T_aux /*aux_val*/)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    const float depthexp = (float)(s1 - center_).length();
#if 0
    // compute appearance probability of observation
    const float exp  = boxm_apm_traits<APM>::apm_processor::expected_color(cell_value.appearance());
#endif
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = std::exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;
    // and update expected image
    expected_(i,j) += depthexp * Omega;
    // update visibility probability
    vis_img_(i,j) = vis_prob_end;
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<float> &expected_;
  vil_image_view<float> &vis_img_;
  vil_image_view<float> alpha_integral_;
  vgl_point_3d<double> center_;
};


//: Functor class to normalize expected image
class normalize_expected_functor_depth_rt
{
 public:
  normalize_expected_functor_depth_rt(float depth_max):depth_max_(depth_max) {}

  void operator()(float mask, float &pix) const
  {
    pix += mask*depth_max_;
  }

 private:
  float depth_max_;
};


template <class T_loc, class T_data>
void boxm_render_depth_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                          vpgl_camera_double_sptr cam,
                          vil_image_view<float> &expected,
                          vil_image_view<float> &mask,
                          int bin = -1,                      // FIXME - unused
                          bool use_black_background = false) // FIXME - unused
{
  if (vpgl_perspective_camera<double> *pcam=dynamic_cast<vpgl_perspective_camera<double> *>(cam.ptr()))
  {
    typedef boxm_aux_traits<BOXM_AUX_NULL>::sample_datatype sample_datatype;
    boxm_aux_scene<T_loc, T_data, boxm_rt_sample<sample_datatype> > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_NULL>::storage_subdir(), boxm_aux_scene<T_loc, T_data, boxm_rt_sample<sample_datatype> >::LOAD);

    typedef boxm_render_expected_depth_functor<T_data::apm_type,boxm_rt_sample<sample_datatype> > expfunctor;
    boxm_raytrace_function<expfunctor,T_loc, T_data,boxm_rt_sample<sample_datatype> > raytracer(scene,aux_scene,cam.ptr(),expected.ni(),expected.nj());
    expfunctor exp_functor(expected,mask,pcam->camera_center(),expected.ni(),expected.nj(),true,false);
    raytracer.run(exp_functor);

    float max_depth=(float)boxm_utils::max_point_to_box_dist(scene.get_world_bbox(),pcam->camera_center());

    normalize_expected_functor_depth_rt norm_fn(max_depth);
    vil_transform2<float,float, normalize_expected_functor_depth_rt >(mask,expected,norm_fn);
  }
}

#endif // boxm_render_expected_depth_functor_h
