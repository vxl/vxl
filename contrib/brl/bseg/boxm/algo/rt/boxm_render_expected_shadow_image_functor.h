#ifndef boxm_render_expected_shadow_image_functor_h
#define boxm_render_expected_shadow_image_functor_h
//:
// \file
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/boxm_aux_scene.h>

#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/boxm_opt2_sample.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <boxm/sample/algo/boxm_mob_grey_processor.h>

#include <vil/vil_transform.h>

template <class T_loc, boxm_apm_type APM, boxm_aux_type AUX>
class boxm_render_expected_shadow_image_functor
{
 public:
  typedef typename boxm_aux_traits<AUX>::sample_datatype T_aux;
  //: "default" constructor
  boxm_render_expected_shadow_image_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected,
                                            vil_image_view<float> &mask_vis,
                                            unsigned int ni,unsigned nj,
                                            bool scene_read_only=false,bool is_aux=true)
    : expected_(expected), vis_img_(mask_vis), alpha_integral_(ni,nj, 1)
  {
    alpha_integral_.fill(0.0f);
    typename boxm_apm_traits<APM>::obs_datatype nil(0);
    expected_.fill(nil);
    vis_img_.fill(1.0f);
    scene_read_only_=scene_read_only;
    is_aux_=is_aux;
  }

  inline bool step_cell(unsigned int i, unsigned int j,
                        vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> &cell_value,
                        T_aux& aux_val)
  {
    // compute segment length
    const float seg_len = (float)(s1 - s0).length();
    // compute appearance probability of observation
    const float exp  = boxm_apm_traits<APM>::apm_processor::expected_color(cell_value.appearance());

    //compute sun visibility
    float length_sum = aux_val.seg_len_;
    float avg_sun_vis = 1.0f;
    if (length_sum>0.0f)
      avg_sun_vis = aux_val.vis_/length_sum;

    // update alpha integral
    alpha_integral_(i,j) += cell_value.alpha * seg_len;

    // compute new visibility probability with updated alpha_integral
    const float vis_prob_end = std::exp(-alpha_integral_(i,j));
    // compute weight for this cell
    const float Omega = vis_img_(i,j) - vis_prob_end;
    // and update expected image
    expected_(i,j) +=  exp * Omega * avg_sun_vis;
    // update visibility probability
    vis_img_(i,j) = vis_prob_end;
    return true;
  }

  bool scene_read_only_;
  bool is_aux_;

 private:
  vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &expected_;
  vil_image_view<float> &vis_img_;
  vil_image_view<float> alpha_integral_;
};


//: Functor class to normalize expected image
template<class T_data>
class normalize_shadow_functor
{
 public:

  normalize_shadow_functor(bool use_black_background)
    {use_black_background_ = use_black_background;}

  void operator()(float mask, typename T_data::obs_datatype &pix) const
  {
    if (!use_black_background_) {
      pix += mask*0.5f;
    }
  }
  bool use_black_background_;
};


template <class T_loc, boxm_apm_type APM ,  boxm_aux_type AUX>
void boxm_render_shadow_image_rt(boxm_scene<boct_tree<T_loc, boxm_sample<APM> > > &scene,
                                 std::string const& aux_scene_id,
                                 vpgl_camera_double_sptr cam,
                                 vil_image_view<typename boxm_sample<APM>::obs_datatype> &expected,
                                 vil_image_view<float> & mask,
                                 bool use_black_background = false)
{
  typedef boxm_sample<APM> T_data;
  typedef typename boxm_aux_traits<AUX>::sample_datatype T_aux;
  boxm_aux_scene<T_loc, T_data, T_aux> aux_scene(&scene, aux_scene_id, boxm_aux_scene<T_loc, T_data, T_aux>::LOAD);

  typedef boxm_render_expected_shadow_image_functor<T_loc, APM, AUX > exp_funct;

  boxm_raytrace_function<exp_funct, T_loc, T_data, T_aux > raytracer(scene,aux_scene,cam.ptr(),expected.ni(),expected.nj());
  exp_funct expf(expected,mask,expected.ni(),expected.nj(),true,true);
  raytracer.run(expf);

  normalize_shadow_functor<T_data> norm_fn(use_black_background);
  vil_transform2<float,typename T_data::obs_datatype, normalize_shadow_functor<T_data> >(mask,expected,norm_fn);

  //  aux_scene.clean_scene();
}

#endif
