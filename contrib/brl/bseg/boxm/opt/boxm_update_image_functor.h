#ifndef boxm_update_image_functor_h
#define boxm_update_image_functor_h
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
class boxm_update_image_functor_pass_0
{
 public:
  boxm_update_image_functor_pass_0(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &observation,
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
class boxm_update_image_functor_pass_1
{
 public:
  //: "default" constructor
  boxm_update_image_functor_pass_1(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                                   vil_image_view<float> &pre_inf, vil_image_view<float> &vis_inf)
    : obs_(image), vis_img_(vis_inf), pre_img_(pre_inf), alpha_integral_(image.ni(), image.nj(), 1)
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
    aux_val.vis_+=seg_len*vis_img_(i,j);
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
class boxm_update_image_functor_pass_2
{
 public:
  //: "default" constructor
  boxm_update_image_functor_pass_2(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                                   vil_image_view<float> const& normalizing_img)
  : obs_(image), norm_(normalizing_img), vis_img_(image.ni(),image.nj(),1), pre_img_(image.ni(),image.nj(),1), alpha_integral_(image.ni(),image.nj(),1)
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
    // update vis and pre
    pre_img_(i,j) +=  PI * Omega;
    vis_img_(i,j) = vis_prob_end;

    aux_val.updatefactor_+=(((pre_img_(i,j)+vis_img_(i,j)*PI)/norm_(i,j))* seg_len);
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& obs_;
  vil_image_view<float> const& norm_;
  //vil_image_view<float> const& pre_inf_;
  vil_image_view<float> vis_img_;
  vil_image_view<float> pre_img_;
  vil_image_view<float> alpha_integral_;
};

template <boxm_apm_type APM, class T_aux>
class boxm_update_image_functor_pass_3
{
 public:
  //: default constructor
  boxm_update_image_functor_pass_3()
  {
    //only reads info from the scene
    scene_read_only_=false;
    //needs to write aux
    is_aux_=true;
  }

  inline bool step_cell(boxm_sample<APM> &cell_value, T_aux & aux_val)
  {
    if (aux_val.seg_len_>1e-10)
    {
      boxm_apm_traits<APM>::apm_processor::update(cell_value.appearance(),
                                                  aux_val.obs_/aux_val.seg_len_,
                                                  aux_val.vis_/aux_val.seg_len_);

      cell_value.alpha*=(aux_val.updatefactor_/aux_val.seg_len_);
      aux_val.seg_len_=0;
      aux_val.obs_=0;
      aux_val.vis_=0;
      aux_val.updatefactor_=0.0;
    }
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;
};

template <class T_loc, class T_data>
void boxm_update_image_rt(boxm_scene<boct_tree<T_loc, T_data > > &scene,
                          vpgl_camera_double_sptr cam,
                          vil_image_view<typename T_data::obs_datatype> &obs,
                          bool black_background = false)
{
    typedef boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype sample_datatype;
    boxm_aux_scene<T_loc, T_data, sample_datatype > aux_scene(&scene,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::storage_subdir(), boxm_aux_scene<T_loc, T_data, sample_datatype>::CLONE);
    typedef boxm_update_image_functor_pass_0<T_data::apm_type,sample_datatype>  pass_0;
    boxm_raytrace_function<pass_0,T_loc, T_data,sample_datatype> raytracer_0(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    vcl_cout<<"PASS 0"<<vcl_endl;
    pass_0 pass_0_functor(obs,obs.ni(),obs.nj());
    raytracer_0.run(pass_0_functor);

    vil_image_view<float> pre_inf(obs.ni(),obs.nj(),1);
    vil_image_view<float> vis_inf(obs.ni(),obs.nj(),1);

    typedef boxm_update_image_functor_pass_1<T_data::apm_type,sample_datatype> pass_1;
    boxm_raytrace_function<pass_1,T_loc, T_data,sample_datatype> raytracer_1(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    vcl_cout<<"PASS 1"<<vcl_endl;
    pass_1 pass_1_functor(obs,pre_inf,vis_inf);
    raytracer_1.run(pass_1_functor);

    vil_image_view<float> PI_inf(obs.ni(), obs.nj(),1);

    typename T_data::apm_datatype background_apm;

    if (black_background) {
        vcl_cout << "using black background model" << vcl_endl;
        for (unsigned int i=0; i<4; ++i) {
            T_data::apm_processor::update(background_apm, 0.0f, 1.0f);
            float peak=T_data::apm_processor::prob_density(background_apm,0.0f);
            vcl_cout<<"Peak: "<<peak<<vcl_endl;
        }
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
    vil_image_view<float> norm_img(obs.ni(), obs.nj());
    vil_math_image_sum<float,float,float>(pre_inf,inf_term,norm_img);
    vcl_cout<<"PASS 2"<<vcl_endl;
    typedef boxm_update_image_functor_pass_2<T_data::apm_type,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> pass_2;
    boxm_raytrace_function<pass_2,T_loc, T_data,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> raytracer_2(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());
    pass_2 pass_2_functor(obs,norm_img);
    raytracer_2.run(pass_2_functor);
    vcl_cout<<"PASS 3"<<vcl_endl;
    typedef boxm_update_image_functor_pass_3<T_data::apm_type,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> pass_3;
    boxm_iterate_cells_function<pass_3,T_loc, T_data,boxm_aux_traits<BOXM_AUX_OPT_RT_GREY>::sample_datatype> cell_tracer_3(scene,aux_scene,cam.ptr(),obs.ni(),obs.nj());

    pass_3 pass_3_functor;
    cell_tracer_3.run(pass_3_functor);

    aux_scene.clean_scene();
    vcl_cout<<"DONE."<<vcl_endl;
}

#endif // boxm_update_image_functor_h
