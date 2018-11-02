// This is brl/bseg/bvxm/pro/processes/bvxm_change_detection_display_process.h
#ifndef bvxm_change_detection_display_process_h_
#define bvxm_change_detection_display_process_h_
//:
// \file
// \brief process to threshold the changes for foreground detection.
//     -  Inputs:
//            * original image
//            * probability image
//            * mask image
//     -  Outputs:
//            * red changes image
//            * probability image in range 0-255
//
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace bvxm_change_detection_display_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 2;
  // The parameters strings
  const std::string param_prob_thresh_ = "prob_thresh";
  const std::string param_prob_image_scale_ = "prob_image_scale";
}

//: sets input and output types for  bvxm_change_detection_display_process
bool bvxm_change_detection_display_process_cons(bprb_func_process& pro);

//: process to threshold the changes for foreground detection
bool bvxm_change_detection_display_process(bprb_func_process& pro);

#endif // bvxm_change_detection_display_process_h_
