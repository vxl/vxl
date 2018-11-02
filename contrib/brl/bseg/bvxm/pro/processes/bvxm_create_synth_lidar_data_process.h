#ifndef bvxm_create_synth_lidar_data_process_h_
#define bvxm_create_synth_lidar_data_process_h_
//:
// \file
// \brief A process for creating synthetic lidar data for testing purposes
//        It basically creates a box in the scene based on the parameter values
//        and fills the lidar image with the height of the box
//
// \author Gamze Tunali
// \date   April 4, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: global variables
namespace bvxm_create_synth_lidar_data_process_globals
{
  // this process takes no inputs
  constexpr unsigned n_outputs_ = 2;
  // Define parameters here
  const std::string param_input_directory_ =  "input_directory";
  const std::string param_voxel_dim_x_ = "voxel_dim_x";
  const std::string param_voxel_dim_y_ = "voxel_dim_y";
  const std::string param_voxel_dim_z_ = "voxel_dim_z";
  const std::string param_box_min_x_ = "box_min_x";
  const std::string param_box_min_y_ = "box_min_y";
  const std::string param_box_min_z_ = "box_min_z";
  const std::string param_box_dim_x_ = "box_dim_x";
  const std::string param_box_dim_y_ = "box_dim_y";
  const std::string param_box_dim_z_ = "box_dim_z";
  const std::string param_lvcs_ = "lvcs";
}

//: creates the synthetic lidar data
bool bvxm_create_synth_lidar_data_process(bprb_func_process& pro);

//: sets input and output types
bool bvxm_create_synth_lidar_data_process_cons(bprb_func_process& pro);

#endif //bvxm_create_synth_lidar_data_process_h_
