//This is brl/bseg/bvxm/pro/processes/bvxm_clean_world_process.cxx

//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>


bool bvxm_clean_world_process(bprb_func_process& pro)
{
  // process takes 1 input:
  //input[0]: The voxel world
  unsigned n_inputs_ = 1;
  if (pro.n_inputs<n_inputs_)
  {
    vcl_cout << pro.name() <<" : The input number should be "<< n_inputs_ << vcl_endl;
    return false;
  }

  //get the inputs:
  unsigned i=0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);

  //check inputs validity
  i = 0;
  if (!world){
    vcl_cout << pro.name() <<" :--  Input " << i++ << " is not valid!\n";
    return false;
  }

  return world->clean_grids();
}
