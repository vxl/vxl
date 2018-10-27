// This is brl/bseg/bvxm/pro/processes/bvxm_save_occupancy_raw_process.cxx
#include "bvxm_save_occupancy_raw_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_save_occupancy_raw_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_raw_process_globals;
  // process takes 3 inputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to
  //input[2]: scale of the voxel default is 0.
  //input[3]: the appearence model as defined in bvxm_voxel_traits.h
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";
  return pro.set_input_types(input_types_);
}

bool bvxm_save_occupancy_raw_process(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_raw_process_globals;

  if (pro.n_inputs()<n_inputs_){
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string filename = pro.get_input<std::string>(i++);
  auto scale = pro.get_input<unsigned>(i++);
  std::string apm = pro.get_input<std::string>(i++);

  if ( !world ){
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  if (apm == "ocp")
    return world->save_occupancy_raw<OCCUPANCY>(filename,scale);
  else if (apm == "ocp_opinion")
    return world->save_occupancy_raw<OCCUPANCY_OPINION>(filename,scale);
  else if (apm == "lidar")
    return world->save_occupancy_raw<LIDAR>(filename,scale);
  else if (apm == "edges")
    return world->save_occupancy_raw<EDGES>(filename,scale);
  else if (apm == "float")
    return world->save_occupancy_raw<FLOAT>(filename,scale);
  else {
    std::cout << "bvxm_save_occupancy_raw_process: The appearence model [" << apm << "]is not defined!" << std::endl;
    return false;
  }
}
