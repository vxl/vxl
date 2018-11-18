//This is brl/bseg/bvxm/pro/processes/bvxm_gen_synthetic_world_process.cxx
#include "bvxm_gen_synthetic_world_process.h"
//:
// \file

#include "bvxm_synth_world_generator.h"
#include <brdb/brdb_value.h>

#include <bprb/bprb_parameters.h>


//:set input and output types
bool bvxm_gen_synthetic_world_process_cons(bprb_func_process& pro)
{
  //process has 1 output
  //output[0] : The voxel_world
  std::vector<std::string> output_types_(1);
  output_types_[0] = "bvxm_voxel_world_sptr";
  pro.set_output_types(output_types_);
  return true;
}

//:generates a synthetic world
bool bvxm_gen_synthetic_world_process(bprb_func_process& pro)
{
  auto *world_gen = new bvxm_synth_world_generator();
  bvxm_voxel_world_sptr world = world_gen->generate_world();

  //store output
  pro.set_output_val<bvxm_voxel_world_sptr>(0,world);
  std::cout << "synth world params " << *world->get_params();
  return true;
}
