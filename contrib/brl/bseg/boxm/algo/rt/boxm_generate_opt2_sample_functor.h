#ifndef boxm_generate_opt2_sample_functor_h
#define boxm_generate_opt2_sample_functor_h
//:
// \file
#include <iostream>
#include "boxm_pre_infinity_functor.h"
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_opt2_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/basic/boxm_seg_length_functor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


template <boxm_apm_type APM, class T_aux>
class boxm_generate_opt2_sample_functor_pass_2
{
 public:
  //: "default" constructor
  boxm_generate_opt2_sample_functor_pass_2(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                                           vil_image_view<float> const& beta_denom,
                                           float model_prior,
                                           vil_image_view<float> const& alt_prob_img)
    : scene_read_only_(true) /*only reads info from the scene*/, is_aux_(true) /*needs to write aux*/,
      model_prior_(model_prior), alt_prob_img_(alt_prob_img), obs_(image), beta_denom_(beta_denom),
      vis_img_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1)
  {
    vis_img_.fill(1.0f);
    pre_img_.fill(0.0f);
    alpha_integral_.fill(0.0f);
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1, boxm_sample<APM> &cell_value, T_aux & aux_val)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    float mean_obs = aux_val.obs_/aux_val.seg_len_;
    const float PI  = boxm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance(), mean_obs);
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = std::exp(-alpha_integral_(i,j));
    // grab this cell's pre and vis value
    const float pre = pre_img_(i,j);
    const float vis = vis_img_(i,j);
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;

    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;
    // accumulate aux sample values
    aux_val.pre_ += (pre * model_prior_ + alt_prob_img_(i,j)) * seg_len;
    aux_val.vis_ += vis * model_prior_ * seg_len;

    const float beta_num = pre + vis*PI;
    const float beta_num_expanded = (beta_num  * model_prior_) + alt_prob_img_(i,j);
    const float beta_denom = beta_denom_(i,j);
    const float beta_denom_expanded = (beta_denom * model_prior_) + alt_prob_img_(i,j);

    float beta = 1.0f;
    if (beta_denom_expanded > 1e-5f) {
      beta =  beta_num_expanded / beta_denom_expanded;
    }
    const float old_PQ = (float)(1.0 - std::exp(-cell_value.alpha*seg_len));
    const float new_PQ = old_PQ * beta;
    const float pass_prob_old = 1.0f - old_PQ;
    float pass_prob = 1.0f - new_PQ;

    // compute expected information gained from update
    const float weight = new_PQ * std::log(new_PQ / old_PQ) + pass_prob * std::log(pass_prob / pass_prob_old);
    // ensure log doesn't go to infinity
    if (pass_prob < 1e-5f) {
      pass_prob = 1e-5f;
    }
    aux_val.log_pass_prob_sum_ += std::log(pass_prob) * weight/seg_len;
    aux_val.weighted_seg_len_sum_ += weight; // += seg_len * weight/seg_len;

    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  float model_prior_;
  vil_image_view<float> const& alt_prob_img_;
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> const& beta_denom_;

  //vil_image_view<float> const& pre_inf_;
  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;
};

template <class T_loc, class T_data, boxm_aux_type AUX_T>
void boxm_generate_opt2_samples(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                vpgl_camera_double_sptr cam,
                                vil_image_view<typename T_data::obs_datatype> &obs,
                                std::string iname,
                                std::vector<float> const& alt_appearance_priors,
                                std::vector<typename T_data::apm_datatype> const& alt_appearance_models,
                                bool black_background = false)
{
  typedef typename boxm_aux_traits<AUX_T>::sample_datatype sample_datatype;
  //std::cout << "scene.save_platform_independent() = " << scene.save_platform_independent() << std::endl;
  boxm_aux_scene<T_loc, T_data,  sample_datatype> aux_scene(&scene,iname, boxm_aux_scene<T_loc, T_data,  sample_datatype>::CLONE);
  typedef boxm_seg_length_functor<T_data::apm_type,sample_datatype>  pass_0;
  boxm_raytrace_function<pass_0,T_loc, T_data, sample_datatype> raytracer_0(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  std::cout<<"PASS 0"<<std::endl;
  pass_0 pass_0_functor(obs,obs.ni(),obs.nj());
  raytracer_0.run(pass_0_functor);

  vil_image_view<float> pre_inf(obs.ni(),obs.nj(),1);
  vil_image_view<float> vis_inf(obs.ni(),obs.nj(),1);
  vil_image_view<float> inf_term(obs.ni(), obs.nj());

  typedef boxm_pre_infinity_functor<T_data::apm_type,sample_datatype> pass_1;
  boxm_raytrace_function<pass_1,T_loc, T_data, sample_datatype> raytracer_1(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  std::cout<<"PASS 1"<<std::endl;
  pass_1 pass_1_functor(obs,pre_inf,vis_inf);
  raytracer_1.run(pass_1_functor);

  // compute observation probability for "infinity" appearance model
  if (black_background) {
    std::cout << "using black background model" << std::endl;
    // use single-mode gaussian centered at 0
    typedef bsta_gaussian_sphere<typename T_data::obs_mathtype, T_data::obs_dim> gauss_type;
    typename T_data::obs_mathtype black(0);
    const float black_std_dev = 8.0f/255;
    const gauss_type appearance_dist(black, black_std_dev*black_std_dev);

    //float peak=T_data::apm_processor::expected_color(background_apm);
    //std::cout<<"Peak: "<<peak<<std::endl;
    vil_image_view<float> PI_inf(obs.ni(), obs.nj(),1);
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
  vil_image_view<float> beta_denom_img(obs.ni(), obs.nj());
  vil_math_image_sum<float,float,float>(pre_inf,inf_term,beta_denom_img);

  // compute model prior
  float model_prior = 1.0f;
  for (float alt_appearance_prior : alt_appearance_priors) {
    model_prior -= alt_appearance_prior;
  }
  // sanity check
  if (model_prior <= 0.0f) {
    std::cerr << "error: boxm_generate_opt2_samples : alt_appearance_priors sum to " << 1.0f - model_prior << " >= 1.0!\n";
  }

  // compute alternate appearance probability for each pixel in the image
  vil_image_view<float> alt_prob_img(obs.ni(), obs.nj());
  alt_prob_img.fill(0.0f);
  const unsigned int n_alt = alt_appearance_priors.size();
  if (n_alt > 0) {
    typename vil_image_view<typename T_data::obs_datatype>::const_iterator img_it = obs.begin();
    typename vil_image_view<float>::iterator alt_prob_it = alt_prob_img.begin();
    for (; img_it != obs.end(); ++img_it, ++alt_prob_it) {
      for (unsigned int a=0; a< n_alt; ++a) {
        const float alt_prob_density = boxm_apm_traits<T_data::apm_type>::apm_processor::prob_density(alt_appearance_models[a],*img_it);
        *alt_prob_it +=  alt_prob_density * alt_appearance_priors[a];
      }
    }
  }

  // run the raytrace function
  std::cout<<"PASS 2"<<std::endl;
  typedef boxm_generate_opt2_sample_functor_pass_2<T_data::apm_type, sample_datatype> pass_2;
  boxm_raytrace_function<pass_2,T_loc, T_data, sample_datatype> raytracer_2(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  pass_2 pass_2_functor(obs,beta_denom_img, model_prior, alt_prob_img);
  raytracer_2.run(pass_2_functor);

  //aux_scene.clean_scene();
  std::cout<<"DONE."<<std::endl;
}

#endif // boxm_generate_opt2_sample_functor_h
