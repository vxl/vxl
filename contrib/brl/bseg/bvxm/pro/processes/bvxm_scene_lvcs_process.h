// This is brl/bseg/bvxm/pro/processes/bvxm_scene_lvcs_process.h
#ifndef bvxm_scene_lvcs_process_h_
#define bvxm_scene_lvcs_process_h_
//:
// \file
// \brief A class that returns the bvxm lvcs from scene xml file
//          -  Inputs:
//               * bvxm_voxel_world_sptr
//
//          -  Outputs:
//               * vpgl_lvcs_sptr
//
//
// \author Yi Dong
// \date   March 13, 2015
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_world_params.h>

//: global variables and functions
namespace bvxm_scene_lvcs_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}
//: set input and output types
bool bvxm_scene_lvcs_process_cons(bprb_func_process& pro);
//: process
bool bvxm_scene_lvcs_process(bprb_func_process& pro);

#endif // bvxm_scene_lvcs_process_h_
