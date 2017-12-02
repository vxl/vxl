//This is brl/bseg/bvxm/pro/processes/bvxm_clean_world_process.cxx
#include "bvxm_clean_world_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>

//:sets input and output types for bvxm_clean_world_process
bool bvxm_clean_world_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_clean_world_process_globals;
  //input[0]: The voxel world
  std::vector<std::string> input_types_(n_inputs_);
  input_types_.resize(1);
  input_types_[0] = "bvxm_voxel_world_sptr";
  return pro.set_input_types(input_types_);
};

//:cleans the voxel world
bool bvxm_clean_world_process(bprb_func_process& pro)
{
  using namespace bvxm_clean_world_process_globals;
  // process takes 1 input:
  //input[0]: The voxel world
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() <<" : The input number should be "<< n_inputs_ << std::endl;
    return false;
  }

  //get the inputs:
  unsigned i=0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);

  //check inputs validity
  i = 0;
  if (!world){
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  return world->clean_grids();
}
