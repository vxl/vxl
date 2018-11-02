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
//  Yi Dong  Mar 31 2014  modify to return lower left elev and upper left elev of a scene as well
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bvxm/bvxm_world_params.h>

//: globals variables and functions
namespace bvxm_scene_box_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 6;
}

//: set input and output types
bool bvxm_scene_box_process_cons(bprb_func_process& pro);

bool bvxm_scene_box_process(bprb_func_process& pro);

#endif // bvxm_scene_box_process_h_
