//:
// \file
// \brief A process that computes the "expected visible color"  at each voxel
// \author Isabel Restrepo
// \date 17-Nov-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/algo/boxm_merge_mog.h>


//:global variables
namespace boxm_compute_expected_color_scene_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool boxm_compute_expected_color_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_compute_expected_color_scene_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float"; //offset for appearance values

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm_compute_expected_color_scene_process(bprb_func_process& pro)
{
  using namespace boxm_compute_expected_color_scene_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  auto grey_offset = pro.get_input<float>(1);

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Scene is not valid!\n";
    return false;
  }

  //:Note initial implementation is for fixed types, but this can be changed if more cases are needed

  typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > scene_tree_type;

  auto *scene = dynamic_cast<boxm_scene<scene_tree_type>* > (scene_base.as_pointer());

  //check input's validity
  if (!scene) {
    std::cout <<  " :-- Scene is not of valid type!\n";
    return false;
  }

  //parameters and structure of the output scene are the same as those of the input scene
  boxm_scene<boct_tree<short, float> > *mean_color_scene = new boxm_scene<boct_tree<short, float> >(scene->lvcs(), scene->origin(), scene->block_dim(), scene->world_dim(), scene->max_level(), scene->init_level());
  mean_color_scene->set_paths(scene->path(), "mean_color");
  mean_color_scene->set_appearance_model(BOXM_FLOAT);

  compute_expected_color(*scene, *mean_color_scene, grey_offset);
  mean_color_scene->write_scene("mean_color_scene.xml");
  pro.set_output_val<boxm_scene_base_sptr>(0, mean_color_scene);

  return true;
}
