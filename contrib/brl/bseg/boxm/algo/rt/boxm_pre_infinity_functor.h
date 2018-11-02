#ifndef boxm_pre_infinity_functor_h
#define boxm_pre_infinity_functor_h
//:
// \file
#include <iostream>
#include <boxm/boxm_apm_traits.h>
#include <boxm/basic/boxm_raytrace_function.h>
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <boxm_apm_type APM, class T_aux>
class boxm_pre_infinity_functor
{
 public:
  //: "default" constructor
  boxm_pre_infinity_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> const& image,
                            vil_image_view<float> &pre_inf, vil_image_view<float> &vis_inf)
    : alpha_integral_(image.ni(),image.nj(),1), obs_(image), vis_img_(vis_inf), pre_img_(pre_inf)
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
    const float PI  = boxm_apm_traits<APM>::apm_processor::prob_density(cell_value.appearance(),mean_obs_); // aux_val.PI_;
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

#endif // boxm_pre_infinity_functor_h
