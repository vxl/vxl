//This is brl/bseg/bvxm/pro/processes/bvxm_clean_world_process.cxx
//:
// \file
// \brief A process that deletes all voxel storage files in the world directory. use with caution!
//        Inputs
//             0: The voxel world
//        No outputs
//
// \author Daniel Crispell
// \date 03/09/2008
// \verbatim
//  Modifications
//   Isabel Restrepo - 1/27/09 - converted process-class to functions which is the new design for bvxm_processes.
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>

//:global variables for bvxm_clean_world_process
namespace bvxm_clean_world_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}

//:sets input and output types for bvxm_clean_world_process
bool bvxm_clean_world_process_init(bprb_func_process& pro)
{  
  using namespace bvxm_clean_world_process_globals;
  //input[0]: The voxel world
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_.resize(1);
  input_types_[0] = "bvxm_voxel_world_sptr";

};
  
//:cleans the voxel world
bool bvxm_clean_world_process(bprb_func_process& pro)
{
  using namespace bvxm_clean_world_process_globals;
  // process takes 1 input:
  //input[0]: The voxel world
  if (pro.n_inputs()<n_inputs_)
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
