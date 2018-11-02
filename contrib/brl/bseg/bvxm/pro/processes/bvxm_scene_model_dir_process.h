// This is brl/bseg/bvxm/pro/processes/bvxm_scene_model_dir_process.h
#ifndef bvxm_scene_model_dir_process_h_
#define bvxm_scene_model_dir_process_h_
//:
// \file
// \brief A class that returns the bvxm world directory from scene xml file
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//
//        - Output:
//            * string
//
// \author Yi Dong
// \date   Nov 13, 2013
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

//: global variables and functions
namespace bvxm_scene_model_dir_process_globals
{
  constexpr unsigned  n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

//: set input and output types
bool bvxm_scene_model_dir_process_cons(bprb_func_process& pro);

//: process
bool bvxl_scene_model_dir_process(bprb_func_process& pro);

#endif // bvxm_scene_model_dir_process_h_
