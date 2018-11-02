#ifndef boxm_num_rays_functor_h
#define boxm_num_rays_functor_h
//:
// \file
#include <iostream>
#include <boxm/boxm_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/bsta_histogram.h>
#include <boxm/boxm_apm_traits.h>
#include "boxm_raytrace_function.h"

//: this functor is to compute no of rays passing through a cell
// Works for boxm_scalar_sample
template <boxm_apm_type APM, class T_aux>
class boxm_num_rays_functor
{
 public:
  boxm_num_rays_functor()
  {
    scene_read_only_=true;
    is_aux_=true;
  }

  inline bool step_cell(unsigned int /*i*/, unsigned int /*j*/,  // FIXME: both unused!
                        vgl_point_3d<double> s0, vgl_point_3d<double> s1,
                        boxm_sample<APM> & /*cell_value*/,       // FIXME: unused...
                        T_aux &aux_val)
  {
    const float seg_len = (float)(s1 - s0).length();

    aux_val.scalar_sum_+=1;
    //std::cout<<aux_val.scalar_sum_<<' ';
    aux_val.seg_len_ += seg_len;
    return true;
  }

 public:
  bool scene_read_only_;
  bool is_aux_;
};

#endif
