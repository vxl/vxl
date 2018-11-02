#ifndef boxm_generate_opt3_sample_functor_h
#define boxm_generate_opt3_sample_functor_h
//:
// \file
#include <iostream>
#include "boxm_pre_infinity_functor.h"
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/basic/boxm_seg_length_functor.h>
#include <boxm/sample/boxm_opt3_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <boxm_apm_type APM>
class pre_inf_functor
{
  typedef typename boxm_aux_traits<BOXM_AUX_NULL>::sample_datatype aux_null_t;
 public:
  //: "default" constructor
  pre_inf_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                  vil_image_view<float> &pre_inf, vil_image_view<float> &vis_inf)
    : alpha_integral_(image.ni(),image.nj(),1), obs_(image), vis_img_(vis_inf), pre_img_(pre_inf)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
    //only reads info from the scene
    scene_read_only_=true;
    //no need to write aux
    is_aux_=false;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> &cell_value, aux_null_t & /*aux_val*/)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI  = boxm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance(),obs_(i,j)); // aux_val.PI_;
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = std::exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;
    // and update pre
    pre_img_(i,j) +=  PI * Omega;
    // update visibility probability
    vis_img_(i,j) = vis_prob_end;
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;
  vil_image_view<float> alpha_integral_;
 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> &vis_img_;
  vil_image_view<float> &pre_img_;
};


template <boxm_apm_type APM, class T_aux>
class boxm_generate_opt3_sample_functor
{
 public:
  //: "default" constructor
  boxm_generate_opt3_sample_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                                    vil_image_view<float> const& beta_denom)
    : obs_(image), beta_denom_(beta_denom), vis_img_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
    //only reads info from the scene
    scene_read_only_=true;
    //needs to write aux
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, boxm_sample<APM> &cell_value, T_aux &aux_val)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float PI  = boxm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance(), obs_(i,j));
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = (float)std::exp(-alpha_integral_(i,j));
    // grab this cell's pre and vis value
    const float pre = pre_img_(i,j);
    const float vis = vis_img_(i,j);
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;

    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;

    // update observation distribution
    aux_val.update_obs_dist(obs_(i,j),vis);
    aux_val.weighted_vis_sum_ += vis*seg_len;

    const float old_PQ = 1.0f - (float)std::exp(-cell_value.alpha * seg_len);
    float new_PQ = old_PQ;
    const float beta_denom_val = beta_denom_(i,j);
    const double epsilon = 1e-6;
    if (beta_denom_val > epsilon) {
      const float beta = (pre + vis*PI) / beta_denom_val;
      new_PQ = old_PQ * beta;
    }
    double pass_prob = 1.0 - new_PQ;
    if (pass_prob < epsilon) {
      pass_prob = epsilon;
    }
    aux_val.log_pass_prob_sum_ += (float)std::log(pass_prob);
    aux_val.seg_len_sum_ += seg_len;

    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> const& beta_denom_;

  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;
};

template <class T_loc, class T_data, boxm_aux_type AUX_T>
void boxm_generate_opt3_sample(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                               vpgl_camera_double_sptr cam,
                               vil_image_view<typename T_data::obs_datatype> &obs,
                               std::string image_id,
                               bool black_background = false)
{
  typedef boxm_opt3_sample<boxm_aux_traits<AUX_T>::APM_TYPE> aux_sample_datatype;
  boxm_aux_scene<T_loc, T_data, aux_sample_datatype> aux_scene(&scene, image_id, boxm_aux_scene<T_loc, T_data, aux_sample_datatype>::CLONE);
#if 0
  typedef boxm_seg_length_functor<T_data::apm_type,sample_datatype>  pass_0;
  boxm_raytrace_function<pass_0,T_loc, T_data, sample_datatype> raytracer_0(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  std::cout<<"PASS 0"<<std::endl;
  pass_0 pass_0_functor(obs,obs.ni(),obs.nj());
  raytracer_0.run(pass_0_functor);
#endif
  vil_image_view<float> pre_inf(obs.ni(),obs.nj(),1);
  vil_image_view<float> vis_inf(obs.ni(),obs.nj(),1);

  typedef pre_inf_functor<T_data::apm_type> pre_inf_functor_t;
  boxm_raytrace_function<pre_inf_functor_t, T_loc, T_data> raytracer_1(scene,cam.ptr(),obs.ni(),obs.nj());
  std::cout<<"PASS 1"<<std::endl;
  pre_inf_functor_t pass_1_functor(obs,pre_inf,vis_inf);
  raytracer_1.run(pass_1_functor);

  vil_image_view<float> inf_term(obs.ni(), obs.nj());
  vil_image_view<float> beta_denom_img(obs.ni(), obs.nj());

  // compute observation probability for "infinity" appearance model
  vil_image_view<float> PI_inf(obs.ni(), obs.nj(),1);
  if (black_background) {
    std::cout << "using black background model" << std::endl;
    // use single-mode gaussian centered at 0
    typedef bsta_gaussian_sphere<typename T_data::obs_mathtype, T_data::obs_dim> gauss_type;
    typename T_data::obs_mathtype black(0);
    const float black_std_dev = 8.0f/255;
    const gauss_type appearance_dist(black, black_std_dev);
#if 0
    float peak=T_data::apm_processor::expected_color(background_apm);
    std::cout<<"Peak: "<<peak<<std::endl;
#endif
    typename vil_image_view<typename T_data::obs_datatype>::const_iterator img_it = obs.begin();
    typename vil_image_view<float>::iterator PI_it = PI_inf.begin();
    for (; img_it != obs.end(); ++img_it, ++PI_it) {
      *PI_it = appearance_dist.prob_density(*img_it);
    }
    vil_math_image_product<float,float,float>(vis_inf,PI_inf,inf_term);
  }
  else {
    // assume uniform prob. density on interval [0, 1]
    //PI_inf.fill(1.0f);
    // don't actually have to compute anything since multiplying by 1
    inf_term = vis_inf;
  }
  vil_math_image_sum<float,float,float>(pre_inf,inf_term,beta_denom_img);

#ifdef DEBUG
  vil_save(vis_inf, "E:/tests/capitol/vis_inf.tiff");
  vil_save(pre_inf, "E:/tests/capitol/pre_inf.tiff");
  vil_save(beta_denom_img, "E:/tests/capitol/beta_denom_img.tiff");
  vil_save(pass_1_functor.alpha_integral_, "E:/tests/capitol/alpha_integr.tiff");
#endif

  std::cout<<"PASS 2"<<std::endl;
  typedef boxm_generate_opt3_sample_functor<T_data::apm_type, aux_sample_datatype> pass_2_functor_t;
  boxm_raytrace_function<pass_2_functor_t, T_loc, T_data, aux_sample_datatype> raytracer_2(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  pass_2_functor_t pass_2_functor(obs, beta_denom_img);
  raytracer_2.run(pass_2_functor);
#if 0
  aux_scene.clean_scene();
#endif
  std::cout<<"DONE."<<std::endl;
}

#endif // boxm_generate_opt3_sample_functor_h
