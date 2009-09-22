#ifndef boxm_generate_opt_sample_functor_h
#define boxm_generate_opt_sample_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <vil/vil_math.h>
#include <vcl_iostream.h>

template <boxm_apm_type APM, class T_aux>
class boxm_generate_opt_sample_functor_pass_0
{
 public:
  boxm_generate_opt_sample_functor_pass_0(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &observation,
                                          unsigned int ni,unsigned nj)
    : obs_(observation)
  {
    scene_read_only_=true;
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> &cell_value,T_aux &aux_val)
  {
    const float seg_len = (float)(s1 - s0).length();
    aux_val.obs_ += obs_(i,j) * seg_len;
    aux_val.seg_len_ += seg_len;
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &obs_;
};

template <boxm_apm_type APM, class T_aux>
class boxm_generate_opt_sample_functor_pass_1
{
 public:
  //: "default" constructor
  boxm_generate_opt_sample_functor_pass_1(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                                          vil_image_view<float> &pre_inf, vil_image_view<float> &vis_inf)
    : obs_(image), vis_img_(vis_inf), pre_img_(pre_inf), alpha_integral_(image.ni(),image.nj(),1)
  {
    alpha_integral_.fill(0.0f);
    pre_img_.fill(0.0f);
    vis_img_.fill(1.0f);
    //only reads info from the scene
    scene_read_only_=true;
    //needs to write aux
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> &cell_value, T_aux & aux_val)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute average intensity for the cell
    const float mean_obs_ =aux_val.obs_/aux_val.seg_len_;
    // compute appearance probability of observation
    const float PI  = boxm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance(),mean_obs_);
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;
    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;
    // and update pre
    pre_img_(i,j) +=  PI * Omega;
    // update visibility probabilty
    vis_img_(i,j) = vis_prob_end;
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> &vis_img_;
  vil_image_view<float> &pre_img_;
  vil_image_view<float> alpha_integral_;
};

template <boxm_apm_type APM, class T_aux>
class boxm_generate_opt_sample_functor_pass_2
{
 public:
  //: "default" constructor
  boxm_generate_opt_sample_functor_pass_2(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
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
    float mean_obs= aux_val.obs_/aux_val.seg_len_;
    const float PI  = boxm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance(), mean_obs);
    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = vcl_exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;

    const float pre = pre_img_(i,j);
    const float vis = vis_img_(i,j);

    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;

    aux_val.pre_ += pre * seg_len;
    aux_val.vis_ = vis * seg_len;

    
    const float Beta_num = pre + vis*PI;
    float Beta = 1.0f;
    if (Beta_denom_(i,j) < 1e-6) {
      if (Beta_num > 1e-5) {
        Beta = 10.0f;
      }
    }
    else {
      Beta = Beta_num / Beta_denom_(i,j);
    }
    if (Beta < -1e-5) {
      vcl_cerr << " error: beta = " << Beta << "  setting to 0. " << vcl_endl;
      Beta = 0;
    }
    aux_val.Beta_ += Beta * seg_len;
    //aux_val.seg_len_ += seg_len;
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
void boxm_generate_opt_sample_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                          vpgl_camera_double_sptr cam,
                          vil_image_view<typename T_data::obs_datatype> &obs,
                          vcl_string iname,
                          bool black_background = false)
{
    boxm_aux_scene<T_loc, T_data, boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype > aux_scene(&scene,iname); //boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::storage_subdir());
    typedef boxm_generate_opt_sample_functor_pass_0<T_data::apm_type,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype>  pass_0;
    boxm_raytrace_function<pass_0,T_loc, T_data,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> raytracer_0(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    vcl_cout<<"PASS 0"<<vcl_endl;
    pass_0 pass_0_functor(obs,obs.ni(),obs.nj());
    raytracer_0.run(pass_0_functor);

    vil_image_view<float> pre_inf(obs.ni(),obs.nj(),1);
    vil_image_view<float> vis_inf(obs.ni(),obs.nj(),1);

    typedef boxm_generate_opt_sample_functor_pass_1<T_data::apm_type,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> pass_1;
    boxm_raytrace_function<pass_1,T_loc, T_data,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> raytracer_1(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    vcl_cout<<"PASS 1"<<vcl_endl;
    pass_1 pass_1_functor(obs,pre_inf,vis_inf);
    raytracer_1.run(pass_1_functor);

    vil_image_view<float> PI_inf(obs.ni(), obs.nj(),1);


    if (black_background) {
        vcl_cout << "using black background model" << vcl_endl;
        typename T_data::obs_datatype black(0.0f);
        float background_std_dev = 4.0f/255;
        typename T_data::apm_datatype background_apm(black, background_std_dev,1.0f);
        
        float peak=T_data::apm_processor::expected_color(background_apm);
        vcl_cout<<"Peak: "<<peak<<vcl_endl;
        typename vil_image_view<typename T_data::obs_datatype>::const_iterator img_it = obs.begin();
        typename vil_image_view<float>::iterator PI_it = PI_inf.begin();
        for (; img_it != obs.end(); ++img_it, ++PI_it) {
            *PI_it = T_data::apm_processor::prob_density(background_apm,*img_it);
        }
    }
    else {
        PI_inf.fill(0.5f);
    }

    vil_image_view<float> inf_term(obs.ni(), obs.nj());
    vil_math_image_product<float,float,float>(vis_inf,PI_inf,inf_term);
    vil_image_view<float> Beta_denom_img(obs.ni(), obs.nj());
    vil_math_image_sum<float,float,float>(pre_inf,inf_term,Beta_denom_img);
    vcl_cout<<"PASS 2"<<vcl_endl;
    typedef boxm_generate_opt_sample_functor_pass_2<T_data::apm_type,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> pass_2;
    boxm_raytrace_function<pass_2,T_loc, T_data,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> raytracer_2(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    pass_2 pass_2_functor(obs,Beta_denom_img);
    raytracer_2.run(pass_2_functor);

    //aux_scene.clean_scene();
    vcl_cout<<"DONE."<<vcl_endl;
}

#endif // boxm_generate_opt_sample_functor_h
