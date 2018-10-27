// This is brl/bseg/bvxm/pro/processes/bvxm_save_occupancy_vff_process.cxx
#include "bvxm_save_occupancy_vff_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_save_occupancy_vff_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_vff_process_globals;
  // process takes 3 inputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to
  //input[2]: Scale Index

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  return pro.set_input_types(input_types_);
}

bool bvxm_save_occupancy_vff_process(bprb_func_process& pro)
{
  using namespace bvxm_save_occupancy_vff_process_globals;

  //check number of inputs
  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string filename = pro.get_input<std::string>(i++);
  auto scale_idx = pro.get_input<unsigned>(i++);

  if ( !world ){
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  return world->save_occupancy_vff(filename,scale_idx);
}
