// This is brl/bseg/bvxm/pro/processes/bvxm_update_edges_lidar_process.h
#ifndef bvxm_update_edges_lidar_process_h_
#define bvxm_update_edges_lidar_process_h_
//:
// \file
// \brief A class for update process of edge probabilities in a voxel world using LiDAR data.
//
// \author Ibrahim Eden
// \date July 29, 2008
// \verbatim
//  Modifications
//   Brandon Mayer  - Jan 28, 2009 - converted process-class to function to conform with bvxm_process architecture.
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>

//: global variables
namespace bvxm_update_edges_lidar_process_globals
{
  constexpr unsigned int n_inputs_ = 6;
  constexpr unsigned int n_outputs_ = 0;
}

bool bvxm_update_edges_lidar_process_cons(bprb_func_process& pro);

bool bvxm_update_edges_lidar_process(bprb_func_process& pro);

#endif // bvxm_update_edges_lidar_process_h_
