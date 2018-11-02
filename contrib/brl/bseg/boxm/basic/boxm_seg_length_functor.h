#ifndef boxm_seg_length_functor_h
#define boxm_seg_length_functor_h
//:
// \file
#include <iostream>
#include <boxm/boxm_apm_traits.h>
#include "boxm_raytrace_function.h"
#include <boxm/sample/boxm_rt_sample.h>
#include <boxm/sample/algo/boxm_mog_grey_processor.h>
#include <boxm/sample/algo/boxm_simple_grey_processor.h>
#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <boxm_apm_type APM, class T_aux>
class boxm_seg_length_functor
{
 public:
  boxm_seg_length_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &observation,
                          unsigned int /*ni*/=0, unsigned /*nj*/=0)
    : obs_(observation)
  {
    scene_read_only_=true;
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> & /*cell_value*/,    // FIXME: unused...
                        T_aux &aux_val)
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

#endif // boxm_seg_length_functor_h
