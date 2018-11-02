// This is brl/bseg/bvxm/pro/process/bvxm_scene_local_box_process.h
#ifndef bvxm_scene_local_box_process_h_
#define bvxm_scene_local_box_process_h_
//:
// \file
// \brief A class that returns lower left corner amd upper right corner in scene local coordinates
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//
//        -  Outputs:
//             * doube values
//
// \author  Yi Dong
// \date    Nov 16, 2013
// \verbatim
//  Modifications
// \endverbatim


#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bvxm/bvxm_world_params.h>

//: globals variables and functions
namespace bvxm_scene_local_box_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 7;  // box_min_x, box_min_y, box_max_x, box_max_y, voxel_size, min_z, max_z
}

//: set input and output types
bool bvxm_scene_local_box_process_cons(bprb_func_process& pro);

bool bvxm_scene_local_box_process(bprb_func_process& pro);

#endif // bvxm_scene_local_box_process_h_
