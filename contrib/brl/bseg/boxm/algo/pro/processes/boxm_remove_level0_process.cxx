//:
// \file
// \brief A process to remove level 0 of the octrees
// \author Isabel Restrepo
// \date Nov 23 2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_scene_levels_utils.h>

//:global variables
namespace boxm_remove_level0_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool boxm_remove_level0_process_cons(bprb_func_process& pro)
{
  using namespace boxm_remove_level0_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr"; //scene_in
  input_types_[1] = "vcl_string"; //block_prefix for scene out

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr"; //scene_out

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}


//: the process
bool boxm_remove_level0_process(bprb_func_process& pro)
{
  using namespace boxm_remove_level0_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the number of inputs should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  std::string block_pref = pro.get_input<std::string>(1);

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  if (auto *scene_in = dynamic_cast<boxm_scene<boct_tree<short, float> >* > (scene_base.as_pointer()))
  {
    //clone the input scene
    boxm_scene<boct_tree<short, float> > *scene_out = new boxm_scene<boct_tree<short, float> >(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(),
                                                                                               scene_in->world_dim(), scene_in->max_level(), scene_in->init_level(),
                                                                                               scene_in->load_all_blocks(), scene_in->save_internal_nodes());

    scene_out->set_paths(scene_in->path(), block_pref);
    scene_out->set_appearance_model(scene_in->appearence_model());
    std::cout << "Cloning\n";
    scene_in->clone_blocks(*scene_out);

    std::cout << "Restructuring\n";
    boxm_remove_level_0_leaves(scene_out);
    std::cout << "Done\n";

    //write the scene after the number of levels has been adjusted
    scene_out->set_octree_levels(scene_in->max_level() - 1, scene_in->init_level());
    scene_out->write_scene(block_pref + ".xml");

    pro.set_output_val<boxm_scene_base_sptr>(0, scene_out);
    return true;
  }

  return true;
}
