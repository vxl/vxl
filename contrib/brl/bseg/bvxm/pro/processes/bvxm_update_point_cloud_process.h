// This is brl/bseg/bvxm/pro/processes/bvxm_update_point_cloud_process.h
#ifndef bvxm_update_point_cloud_process_h_
#define bvxm_update_point_cloud_process_h_
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Vishal Jain
// \date Nov 11, 2009

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_update_point_cloud_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}

bool bvxm_update_point_cloud_process_cons(bprb_func_process& pro);

bool bvxm_update_point_cloud_process(bprb_func_process& pro);

#endif // bvxm_update_point_cloud_process_h_
