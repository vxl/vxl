// This is brl/bseg/bvxm/pro/processes/bvxm_create_voxel_world_process.h
#ifndef bvxm_create_voxel_world_process_h_
#define bvxm_create_voxel_world_process_h_
//:
// \file
// \brief A process for creating a new bvxm_voxel_world from scratch
// \author Daniel Crispell
// \date February 06, 2008
//
// \verbatim
//  Modifications
//   Ozge C Ozcanli  Feb 19, 2008 - removed input string and modified to read input directory
//                                  path as a parameter from an XML  should be modified
//                                  to read all the parameters from the same XML file
//   Gamze Tunali    Feb 24, 2008 - added the parameter list to the process.
//                                  It receives all the world parameters as process parameters now
//   Isabel Restrepo Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

//: global variables
namespace bvxm_create_voxel_world_process_globals
{
  // this process takes no inputs
  const unsigned n_outputs_ = 1;
  // Define parameters here
  const vcl_string param_input_directory_ =  "input_directory";
  const vcl_string param_corner_x_ = "corner_x";
  const vcl_string param_corner_y_ = "corner_y";
  const vcl_string param_corner_z_ = "corner_z";
  const vcl_string param_voxel_dim_x_ = "voxel_dim_x";
  const vcl_string param_voxel_dim_y_ = "voxel_dim_y";
  const vcl_string param_voxel_dim_z_ = "voxel_dim_z";
  const vcl_string param_voxel_length_ = "voxel_length";
  const vcl_string param_lvcs_ = "lvcs";
  const vcl_string param_min_ocp_prob_ = "min_ocp_prob";
  const vcl_string param_max_ocp_prob_ = "max_ocp_prob";
  const vcl_string param_max_scale_ = "max_scale";
}

//: sets input and output types
bool bvxm_create_voxel_world_process_cons(bprb_func_process& pro);

//: creates a voxel world
bool bvxm_create_voxel_world_process(bprb_func_process& pro);

#endif // bvxm_create_voxel_world_process_h_
