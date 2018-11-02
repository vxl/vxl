// This is brl/bseg/bvxm/pro/processes/bvxm_scene_kml_process.h
#ifndef bvxm_scene_kml_process_h_
#define bvxm_scene_kml_process_h_
//:
// \file
// \brief A class that writes scene region into a kml file
//        -  Input:
//               * bvxm_voxel_world_sptr
//
// \author Yi Dong
// \date Dec 04, 2013
// \verbatim
//  Modifications
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bkml/bkml_write.h>
#include <bkml/bkml_parser.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>

//: global variables and functions
namespace bvxm_scene_kml_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 0;
}

//: set input and output types
bool bvxm_scene_kml_process_cons(bprb_func_process& pro);
//: actual process implementation
bool bvxm_scene_kml_process(bprb_func_process& pro);

// bvxm_scene_poly_overlap_process
//: check whether the given scene are overlap with a kml polygon
//: global variable
namespace bvxm_scene_poly_overlap_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool bvxm_scene_poly_overlap_process_cons(bprb_func_process& pro);
bool bvxm_scene_poly_overlap_process(bprb_func_process& pro);

#endif // bvxm_scene_kml_process_h_
