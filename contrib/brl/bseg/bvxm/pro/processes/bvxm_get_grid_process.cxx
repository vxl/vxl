//This is brl/bseg/bvxm/pro/processes/bvxm_get_grid_process.cxx
#include "bvxm_get_grid_process.h"
//:
// \file
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/grid/bvxm_voxel_grid_base_sptr.h>

//:sets input and output types for bvxm_clean_world_process

bool bvxm_get_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_get_grid_process_globals;
  //input[0]: The voxel world
  //input[1]: the grid type
  //input[2]: bin index
  //input[3]: scale
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";

  //output[0]: The grid
  std::vector<std::string> output_types_(1);
  output_types_[0]="bvxm_voxel_grid_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  return pro.set_output_types(output_types_);
};

//:cleans the voxel world
bool bvxm_get_grid_process(bprb_func_process& pro)
{
  using namespace bvxm_get_grid_process_globals;
  // process takes 2 input:
  //input[0]: The voxel world
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() <<" : The input number should be "<< n_inputs_ << std::endl;
    return false;
  }

  //get the inputs:
  unsigned i=0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  std::string voxeltype =  pro.get_input<std::string>(i++);
  auto index =  pro.get_input<unsigned>(i++);
  auto scale =  pro.get_input<unsigned>(i++);

  //check inputs validity
  i = 0;
  if (!world){
    std::cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  bvxm_voxel_grid_base_sptr grid = nullptr;
  if (voxeltype == "occupancy") {
    grid = world->get_grid<OCCUPANCY>(index, scale);
  }
  else if (voxeltype == "edges") {
    grid = world->get_grid<EDGES>(index, scale);
  }
  else if (voxeltype == "ocp_opinion"){
    bvxm_voxel_grid_base_sptr grid = world->get_grid<OCCUPANCY_OPINION>(index, scale);
  }
  else if (voxeltype == "apm_mog_grey"){
    bvxm_voxel_grid_base_sptr grid = world->get_grid<APM_MOG_GREY>(index, scale);
  }
  else {
    std::cerr << "datatype not supported\n";
    return false;
  }

  pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
  return true;
}
