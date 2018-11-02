// This is brl/bseg/bvxm/pro/processes/bvxm_lidar_edge_detection_process.h
#ifndef bvxm_lidar_edge_detection_process_h_
#define bvxm_lidar_edge_detection_process_h_
//:
// \file
// \brief A class for generating LiDAR edges given a LiDAR image pair.
//        -  Inputs:
//             * First return path (string)
//             * Second return path (string)
//
//        -  Outputs:
//             * clipped image area (first ret) "vil_image_view_base_sptr"
//             * clipped image area (second ret) "vil_image_view_base_sptr"
//             * mask "vil_image_view_base_sptr"
//
// \author  Ibrahim Eden
// \date    July 31, 2008
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

//: global variables/functions
namespace bvxm_lidar_edge_detection_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 3;

  // set parameter identifying strings
  const std::string param_thresh_diff_ =  "threshold_edge_difference";
}

//: set input and output types
bool bvxm_lidar_edge_detection_process_cons(bprb_func_process& pro);

bool bvxm_lidar_edge_detection_process(bprb_func_process& pro);

#endif // bvxm_lidar_edge_detection_process_h_
