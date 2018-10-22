// This is brl/bseg/bvxm/pro/processes/bvxm_rpc_registration_process.h
#ifndef bvxm_rpc_registration_process_h_
#define bvxm_rpc_registration_process_h_
//:
// \file
// \brief A process that optimizes rpc camera parameters based on edges in images and the voxel world
//
// \author Ibrahim Eden
// \date March 02, 2008
// \verbatim
//  Modifications
//   Brandon Mayer  - Jan 28, 2009 - converted process-class to function to conform with new bvxm_process architecture.
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_rpc_registration_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 4;
}

//: set input and output types
bool bvxm_rpc_registration_process_cons(bprb_func_process& pro);

//:  optimizes rpc camera parameters based on edges
bool bvxm_rpc_registration_process(bprb_func_process& pro);

#endif // bvxm_rpc_registration_process_h_
