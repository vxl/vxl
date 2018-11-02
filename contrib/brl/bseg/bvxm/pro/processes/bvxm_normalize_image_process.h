// This is brl/bseg/bvxm/pro/processes/bvxm_normalize_image_process.h
#ifndef bvxm_normalize_image_process_h_
#define bvxm_normalize_image_process_h_
//:
// \file
// \brief A process for contrast normalization of images using a voxel world.
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Ozge Can Ozcanli
// \date Feb. 13, 2008
// \verbatim
//  Modifications
//   Ozge C Ozcanli - Mar 25, 2008 - fixed a compiler warning as suggested by Daniel Lyddy
//   Isabel Restrepo- Aug 22, 2008 - Moved most of calculations to template function
//                    norm_parameters<bvxm_voxel_type APM_T>.
//                  - Added support for multichannel appearance model processor,
//                  - Removed support for rgb_mog_processor
//
//   Ozge C Ozcanli - Dec 12, 2008 - added a third option to create mixture of gaussians (mog) image by sampling from the mixtures along the ray
//   Isabel Restrepo- Jan 7, 2009 - converted process-class to functions which is the new design for processes.
//   Ozge C Ozcanli - Feb 10, 2009 - MOG creation part is separated as another process so fixed this process to input MOG image as an input of type bvxm_voxel_slab_base_sptr
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>

// global variables/functions for bvxm_normalize_image_process_globals
namespace bvxm_normalize_image_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 3;

  // normalized image = a*(original_image) +b;
  const std::string param_a_start_= "a_start";
  const std::string param_a_inc_ = "a_inc";
  const std::string param_a_end_ = "a_end";
  const std::string param_b_start_ = "b_start";
  const std::string param_b_end_ = "b_end";
  const std::string param_b_ratio_ = "b_ration";
  const std::string param_verbose_ = "verbose";

  // initialize variables that hold paramerters.
  float a_start_ = 0.6f;
  float a_inc_ = 0.05f;
  float a_end_ = 1.8f;
  float b_start_ = 100.0f/255.0f;
  float b_end_ = 5.0f/255.0f;
  float b_ratio_ = 0.5f;
  bool verbose_ = false;

  // other global variables
  unsigned ni_= 0;
  unsigned nj_= 0;
  unsigned nplanes_=0;

  // this processes functions
  template <bvxm_voxel_type APM_T>
  bool norm_parameters(vil_image_view_base_sptr const& input_img,vil_image_view<float>*& input_img_float_stretched,
                       bvxm_voxel_slab_base_sptr const& mog_image,
                       float& a, float& b);
}

//: sets input and output types for bvxm_create_normalized_nplanes_image_process
bool bvxm_normalize_image_process_cons(bprb_func_process& pro);

bool bvxm_normalize_image_process(bprb_func_process& pro);

#endif // bvxm_normalize_image_process_h_
