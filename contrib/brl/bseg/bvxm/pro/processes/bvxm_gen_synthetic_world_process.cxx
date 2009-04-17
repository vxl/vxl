//This is brl/bseg/bvxm/pro/processes/bvxm_gen_synthetic_world_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for generating a synthetic bvxm_voxel_world.
//
// Adapted from bvxm_test_gen_synthetic_world
//
// \author Isabel Restrepo
// \date Apr 03, 2008
// \verbatim
//  Modifications
//   Ozge C Ozcanli - added parameters
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Isabel Restrepo - 3/10/09 - Moved local functions and variables to processes/bvxm_synth_world_generator.
// \endverbatim

#include "bvxm_synth_world_generator.h"
#include <brdb/brdb_value.h>

#include <bprb/bprb_parameters.h>


//:set input and output types
bool bvxm_gen_synthetic_world_process_cons(bprb_func_process& pro)
{
  //process has 1 output
  //output[0] : The voxel_world
  vcl_vector<vcl_string> output_types_(1);
  output_types_[0] = "bvxm_voxel_world_sptr";
  pro.set_output_types(output_types_);
  return true;
}

//:generates a synthetic world
bool bvxm_gen_synthetic_world_process(bprb_func_process& pro)
{
  bvxm_synth_world_generator *world_gen = new bvxm_synth_world_generator();
  bvxm_voxel_world_sptr world = world_gen->generate_world();
  bvxm_voxel_world_sptr world_recontructed = new bvxm_voxel_world();
  
  //store output
  pro.set_output_val<bvxm_voxel_world_sptr>(0,world);
  vcl_cout << "synth world params " << *world->get_params();
  return true;
}

