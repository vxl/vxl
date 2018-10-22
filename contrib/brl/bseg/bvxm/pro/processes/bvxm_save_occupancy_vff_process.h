// This is brl/bseg/bvxm/pro/processes/bvxm_save_occupancy_vff_process.h
#ifndef bvxm_save_occupancy_vff_process_h_
#define bvxm_save_occupancy_vff_process_h_
//:
// \file
// \brief Save the voxel world occupancy grid in binary format
// A process that saves the voxel world occupancy grid in a binary format
// readable by GE MicroView and other volume renderers
//
// \author Daniel Crispell
// \date May 01, 2008
// \verbatim
//  Modifications
//   Brandon Mayer  - Jan 28, 2009 - converted process-class to function to conform with bvxm_process architecture.
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_save_occupancy_vff_process_globals
{
  constexpr unsigned int n_inputs_ = 3;
  constexpr unsigned int n_outputs_ = 0;
}

bool bvxm_save_occupancy_vff_process_cons(bprb_func_process& pro);

bool bvxm_save_occupancy_vff_process(bprb_func_process& pro);

#endif // bvxm_save_occupancy_vff_process_h_
