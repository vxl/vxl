// This is brl/bseg/bvxm/pro/processes/bvxm_save_edges_raw_process.cxx
#include "bvxm_save_edges_raw_process.h"
//:
// \file

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_edge_ray_processor.h>

namespace bvxm_save_edges_raw_process_globals
{
  unsigned int n_inputs_ = 3;
  unsigned int n_outputs_ = 0;
}

bool bvxm_save_edges_raw_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_save_edges_raw_process_globals;

  // process takes 3 inputs and has no outputs:
  //input[0]: The voxel world
  //input[1]: The filename to write to
  //input[2]: Scale

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool bvxm_save_edges_raw_process(bprb_func_process& pro)
{
  using namespace bvxm_save_edges_raw_process_globals;

  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_edge_ray_processor edge_proc(world);
  std::string filename = pro.get_input<std::string>(i++);
  auto scale = pro.get_input<unsigned>(i++);

  float n_normal = world->get_params()->edges_n_normal();

  if ( !world ){
    std::cout << pro.name() <<" :--  Input 0  is not valid!\n";
    return false;
  }

  return edge_proc.save_edges_raw(filename,n_normal,scale);
}
