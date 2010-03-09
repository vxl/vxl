#ifndef boxm_generate_opt2_sample_functor_h
#define boxm_generate_opt2_sample_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_opt2_sample.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <boxm/opt/boxm_seg_length_functor.h>
#include <boxm/opt/boxm_pre_infinity_functor.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vcl_iostream.h>


template <boxm_apm_type APM, class T_aux>
class boxm_generate_opt2_sample_functor_pass_2
{
public:
  //: "default" constructor
  boxm_generate_opt2_sample_functor_pass_2(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
    vil_image_view<float> const& Beta_denom)
    : obs_(image), Beta_denom_(Beta_denom), vis_img_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
    //only reads info from the scene
    scene_read_only_=true;
    //needs to write aux
    is_aux_=true;
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
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // grab this cell's pre and vis value
    const float pre = pre_img_(i,j);
    const float vis = vis_img_(i,j);
    // compute weight for this cell
    const float Omega = vis - vis_prob_end;

    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;
    // accumulate aux sample values
    aux_val.pre_ += pre * seg_len;
    aux_val.vis_ += vis * seg_len;


    float beta = 1.0f;
    if (Beta_denom_(i,j) > 1e-6f) {
      beta = (pre + vis*PI) / Beta_denom_(i,j);
    }
    const float old_PQ = (float)(1.0 - vcl_exp(-cell_value.alpha*seg_len));
    const float new_PQ = old_PQ * beta;
    const float pass_prob_old = 1.0f - old_PQ;
    float pass_prob = 1.0f - new_PQ;

    // compute expected information gained from update
    const float weight = new_PQ * vcl_log(new_PQ / old_PQ) + pass_prob * vcl_log(pass_prob / pass_prob_old);
    // ensure log doesn't go to infinity
    if (pass_prob < 1e-5f) {
      pass_prob = 1e-5f;
    }
    aux_val.log_pass_prob_sum_ += vcl_log(pass_prob) * weight;
    aux_val.weighted_seg_len_sum_ += seg_len * weight;

    return true;
  }

public:
  bool scene_read_only_;
  bool is_aux_;

private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> const& Beta_denom_;

  //vil_image_view<float> const& pre_inf_;
  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;
};

template <class T_loc, class T_data>
void boxm_generate_opt2_samples(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                               vpgl_camera_double_sptr cam,
                               vil_image_view<typename T_data::obs_datatype> &obs,
                               vcl_string iname,
                               bool black_background = false)
{
  typedef boxm_aux_traits<BOXM_AUX_OPT2_GREY>::sample_datatype sample_datatype;
  boxm_aux_scene<T_loc, T_data,  sample_datatype> aux_scene(&scene,iname, boxm_aux_scene<T_loc, T_data,  sample_datatype>::CLONE);
  typedef boxm_seg_length_functor<T_data::apm_type,sample_datatype>  pass_0;
  boxm_raytrace_function<pass_0,T_loc, T_data, sample_datatype> raytracer_0(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  vcl_cout<<"PASS 0"<<vcl_endl;
  pass_0 pass_0_functor(obs,obs.ni(),obs.nj());
  raytracer_0.run(pass_0_functor);

  vil_image_view<float> pre_inf(obs.ni(),obs.nj(),1);
  vil_image_view<float> vis_inf(obs.ni(),obs.nj(),1);
  vil_image_view<float> inf_term(obs.ni(), obs.nj());

  typedef boxm_pre_infinity_functor<T_data::apm_type,sample_datatype> pass_1;
  boxm_raytrace_function<pass_1,T_loc, T_data, sample_datatype> raytracer_1(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  vcl_cout<<"PASS 1"<<vcl_endl;
  pass_1 pass_1_functor(obs,pre_inf,vis_inf);
  raytracer_1.run(pass_1_functor);

  // compute observation probability for "infinity" appearance model
  if (black_background) {
    vcl_cout << "using black background model" << vcl_endl;
    // use single-mode gaussian centered at 0
    typedef bsta_gaussian_sphere<typename T_data::obs_mathtype, T_data::obs_dim> gauss_type;
    typename T_data::obs_mathtype black(0);
    const float black_std_dev = 8.0f/255;
    const gauss_type appearance_dist(black, black_std_dev);

    //float peak=T_data::apm_processor::expected_color(background_apm);
    //vcl_cout<<"Peak: "<<peak<<vcl_endl;
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


  vcl_cout<<"PASS 2"<<vcl_endl;
  typedef boxm_generate_opt2_sample_functor_pass_2<T_data::apm_type, sample_datatype> pass_2;
  boxm_raytrace_function<pass_2,T_loc, T_data, sample_datatype> raytracer_2(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
  pass_2 pass_2_functor(obs,beta_denom_img);
  raytracer_2.run(pass_2_functor);

  //aux_scene.clean_scene();
  vcl_cout<<"DONE."<<vcl_endl;
}

#endif // boxm_generate_opt2_sample_functor_h
