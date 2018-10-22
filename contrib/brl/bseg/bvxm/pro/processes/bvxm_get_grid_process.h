// This is brl/bseg/bvxm/pro/processes/bvxm_get_grid_process.h
#ifndef bvxm_get_grid_process_h_
#define bvxm_get_grid_process_h_
//:
// \file
// \brief A process that deletes all voxel storage files in the world directory.
//        use with caution!
//     -  Inputs:
//         *   0: The voxel world
//     -  No outputs
//
// \author Daniel Crispell
// \date March 9, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables for bvxm_clean_world_process
namespace bvxm_get_grid_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}

//: sets input and output types for bvxm_clean_world_process
bool bvxm_get_grid_process_cons(bprb_func_process& pro);

//: cleans the voxel world
bool bvxm_get_grid_process(bprb_func_process& pro);

#endif // bvxm_get_grid_process_h_
