// This is brl/bseg/bvxm/pro/processes/bvxm_scene_box_process.h
#ifndef bvxm_scene_box_process_h_
#define bvxm_scene_box_process_h_
//:
// \file
// \brief A class that returns lower left lat, lon and upper right lat, lon of a scene
//        -  Inputs:
//             * bvxm_voxel_world_sptr
//
//        -  Outputs:
//             * doube values
//
// \author  Ozge C. Ozcanli
// \date    Aug 25, 2013
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

#include <bvxm/bvxm_world_params.h>

//: globals variables and functions
namespace bvxm_scene_box_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 4;
}

//: set input and output types
bool bvxm_scene_box_process_cons(bprb_func_process& pro);

bool bvxm_scene_box_process(bprb_func_process& pro);

#endif // bvxm_scene_box_process_h_
