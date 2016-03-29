// This is brl/bseg/bvxm/pro/processes/bvxm_scene_model_dir_process.cxx
#include "bvxm_scene_model_dir_process.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_util.h>
#include <bprb/bprb_parameters.h>

//: set input and output type
bool bvxm_scene_model_dir_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_scene_model_dir_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";     // voxel world sptr

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vcl_string";                // voxel world dir

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bvxm_scene_model_dir_process(bprb_func_process& pro)
{
  using namespace bvxm_scene_model_dir_process_globals;

  // sanity check input
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the input
  unsigned i = 0;
  bvxm_voxel_world_sptr voxel_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_world_params_sptr params = voxel_world->get_params();

  std::string model_dir = params->model_dir();

  // store the output
  unsigned j = 0;
  pro.set_output_val<std::string>(j++, model_dir);

  return true;
}
