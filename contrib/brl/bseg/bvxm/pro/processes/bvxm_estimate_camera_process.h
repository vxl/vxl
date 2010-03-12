// This is brl/bseg/bvxm/pro/processes/bvxm_estimate_camera_process.h
#ifndef bvxm_estimate_camera_process_h_
#define bvxm_estimate_camera_process_h_
//:
// \file
// \brief A process that optimizes camera parameters based on edges in images and the voxel world
//
// \author Ibrahim Eden
// \date February 11, 2010
// \verbatim
//  Modifications
// \endverbatim

#include <bprb/bprb_func_process.h>

//: globals
namespace bvxm_estimate_camera_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 3;
}

//: set input and output types
bool bvxm_estimate_camera_process_cons(bprb_func_process& pro);

//:  optimizes camera parameters based on edges
bool bvxm_estimate_camera_process(bprb_func_process& pro);

#endif // bvxm_estimate_camera_process_h_
