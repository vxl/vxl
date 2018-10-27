//:
// \file
// \brief A process to label cells at a given leve
// \author Isabel Restrepo
// \date 15-Sep-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <boxm/algo/boxm_scene_levels_utils.h>


//:global variables
namespace boxm_explore_cells_at_level_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool boxm_explore_cells_at_level_process_cons(bprb_func_process& pro)
{
  using namespace boxm_explore_cells_at_level_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr"; //scene in
  input_types_[1] = "unsigned"; //octree level

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr"; //label scene

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm_explore_cells_at_level_process(bprb_func_process& pro)
{
  using namespace boxm_explore_cells_at_level_process_globals;

  //get inputs
  boxm_scene_base_sptr scene_in_base = pro.get_input<boxm_scene_base_sptr>(0);
  auto level = pro.get_input<unsigned>(1);

  if (scene_in_base->appearence_model() == BOXM_FLOAT)
  {
    typedef boct_tree<short, float > type;

    auto* scene = dynamic_cast<boxm_scene<type>*>(scene_in_base.as_pointer());

    if (!scene)
      return false;

    //store output
    pro.set_output_val<boxm_scene_base_sptr>(0, boxm_explore_cells_at_level(scene,level));
  }

  return true;
}
