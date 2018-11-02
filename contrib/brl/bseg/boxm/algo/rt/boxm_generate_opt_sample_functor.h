#ifndef boxm_generate_opt_sample_functor_h
#define boxm_generate_opt_sample_functor_h
//:
// \file
#include <iostream>
#include "boxm_pre_infinity_functor.h"
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/basic/boxm_seg_length_functor.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vcl_compiler.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif


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
    aux_val.pre_ += pre * seg_len;
    aux_val.vis_ += vis * seg_len;

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
    if (Beta < 0) {
      if (Beta < -1e-5) {
        std::cerr << " error: beta = " << Beta << "  setting to 0. " << '\n';
      }
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

  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;
};

template <class T_loc, class T_data>
void boxm_generate_opt_sample_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                                 vpgl_camera_double_sptr cam,
                                 vil_image_view<typename T_data::obs_datatype> &obs,
                                 std::string iname,
                                 bool black_background = false)
{
    typedef boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype sample_datatype;
    boxm_aux_scene<T_loc, T_data,  sample_datatype> aux_scene(&scene,iname, boxm_aux_scene<T_loc, T_data,  sample_datatype>::CLONE);
    typedef boxm_seg_length_functor<T_data::apm_type,sample_datatype>  pass_0;
    boxm_raytrace_function<pass_0,T_loc, T_data, sample_datatype> raytracer_0(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    std::cout<<"PASS 0"<<std::endl;
    pass_0 pass_0_functor(obs,obs.ni(),obs.nj());
    raytracer_0.run(pass_0_functor);

    vil_image_view<float> pre_inf(obs.ni(),obs.nj(),1);
    vil_image_view<float> vis_inf(obs.ni(),obs.nj(),1);

    typedef boxm_pre_infinity_functor<T_data::apm_type,sample_datatype> pass_1;
    boxm_raytrace_function<pass_1,T_loc, T_data, sample_datatype> raytracer_1(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    std::cout<<"PASS 1"<<std::endl;
    pass_1 pass_1_functor(obs,pre_inf,vis_inf);
    raytracer_1.run(pass_1_functor);

    vil_image_view<float> PI_inf(obs.ni(), obs.nj(),1);


    if (black_background) {
        std::cout << "using black background model" << std::endl;
        typename T_data::obs_datatype black(0.0f);
        float background_std_dev = 8.0f/255; // ???????????? 4/255 Vishal's
        typename T_data::apm_datatype background_apm(black, background_std_dev*background_std_dev,1.0f);

        float peak=T_data::apm_processor::expected_color(background_apm);
        std::cout<<"Peak: "<<peak<<std::endl;
        typename vil_image_view<typename T_data::obs_datatype>::const_iterator img_it = obs.begin();
        typename vil_image_view<float>::iterator PI_it = PI_inf.begin();
        for (; img_it != obs.end(); ++img_it, ++PI_it) {
            *PI_it = T_data::apm_processor::prob_density(background_apm,*img_it);
        }
    }
    else {
        PI_inf.fill(1.0f);
    }

    vil_image_view<float> inf_term(obs.ni(), obs.nj());
    vil_math_image_product<float,float,float>(vis_inf,PI_inf,inf_term);
    vil_image_view<float> Beta_denom_img(obs.ni(), obs.nj());
    vil_math_image_sum<float,float,float>(pre_inf,inf_term,Beta_denom_img);
#ifdef DEBUG
    vil_save(vis_inf, "E:/tests/capitol/vis_inf.tiff");
    vil_save(pre_inf, "E:/tests/capitol/pre_inf.tiff");
    vil_save(Beta_denom_img, "E:/tests/capitol/Beta_denom_img.tiff");
    vil_save(pass_1_functor.alpha_integral_, "E:/tests/capitol/alpha_integr.tiff");
#endif
    std::cout<<"PASS 2"<<std::endl;
    typedef boxm_generate_opt_sample_functor_pass_2<T_data::apm_type, sample_datatype> pass_2;
    boxm_raytrace_function<pass_2,T_loc, T_data, sample_datatype> raytracer_2(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    pass_2 pass_2_functor(obs,Beta_denom_img);
    raytracer_2.run(pass_2_functor);

#if 0
    aux_scene.clean_scene();
#endif
    std::cout<<"DONE."<<std::endl;
}

#endif // boxm_generate_opt_sample_functor_h
