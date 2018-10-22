// This is brl/bseg/bvxm/pro/processes/bvxm_compare_3d_voxels_process.h
#ifndef bvxm_compare_3d_voxels_process_h_
#define bvxm_compare_3d_voxels_process_h_
//:
// \file
// \author Vishal Jain
// \date April 15, 2008
//
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_compare_3d_voxels_process_globals
{
  constexpr unsigned int n_inputs_ = 4;
  constexpr unsigned int n_outputs_ = 0;
}

//: set input and output types
bool bvxm_compare_3d_voxels_process_cons(bprb_func_process& pro);

//: compares voxel worlds
bool bvxm_compare_3d_voxels_process(bprb_func_process& pro);

#endif // bvxm_compare_3d_voxels_process_h_
