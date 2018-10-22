// This is brl/bseg/bvxm/pro/processes/bvxm_create_local_rpc_process.h
#ifndef bvxm_create_local_rpc_process_h_
#define bvxm_create_local_rpc_process_h_
//:
// \file
// \brief A process that takes a world model and rational camera and returns a local rational camera
//  Inputs:
//  * 0: The voxel world
//  * 1: The current camera
//  Outputs:
//  * 0: The local camera
//
// \author Ibrahim Eden
// \date March 14, 2008
//
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_create_local_rpc_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}

//: initialize input and output types
bool bvxm_create_local_rpc_process_cons(bprb_func_process& pro);

//: process that takes a world model and rational camera and returns a local rational camera
bool bvxm_create_local_rpc_process(bprb_func_process& pro);

#endif // bvxm_create_local_rpc_process_h_
