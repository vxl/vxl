#ifndef boxm_merge_mog_functor_h
#define boxm_merge_mog_functor_h
//:
// \file
// \brief class with methods to merge MOGs (Mixtures of Gaussians).
// \author Isabel Restrepo
// \date Nov 30, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <bsta/bsta_distribution.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture_fixed.h>
#include <bsta/bsta_gaussian_indep.h>

#include <boxm/boxm_apm_traits.h>
#include <boxm/opt/boxm_raytrace_function.h>
#include <boxm/opt/boxm_rt_sample.h>
#include <boxm/boxm_mog_grey_processor.h>
#include <boxm/boxm_simple_grey_processor.h>
#include <vil/vil_math.h>
#include <vcl_iostream.h>

template <enum boxm_apm_type APM>
class boxm_merge_mog_functor
{
 public:
  boxm_merge_mog_functor(vil_image_view<typename boxm_apm_traits<APM>::obs_datatype> &observation,
                         unsigned int ni,unsigned nj)
    : obs_(observation)
  {
    scene_read_only_=true;
    is_aux_=true;
  }

  inline bool step_cell(unsigned int i, unsigned int j, vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> &cell_value, typename boxm_apm_traits<APM>::obs_datatype &aux_val)
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

#endif // boxm_merge_mog_functor_h
