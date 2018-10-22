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
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 3;

  // parameter strings
  const std::string theta_range_ =  "theta_range";
  const std::string theta_step_ =  "theta_step";
  const std::string phi_range_ =  "phi_range";
  const std::string phi_step_ =  "phi_step";
  const std::string rot_range_ =  "rot_range";
  const std::string rot_step_ =  "rot_step";
  const std::string max_iter_rot_angle_ =  "max_iter_rot_angle";
  const std::string max_iter_cam_center_ =  "max_iter_cam_center";
}

//: set input and output types
bool bvxm_estimate_camera_process_cons(bprb_func_process& pro);

//:  optimizes camera parameters based on edges
bool bvxm_estimate_camera_process(bprb_func_process& pro);

#endif // bvxm_estimate_camera_process_h_
