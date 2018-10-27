//:
// \file
// \brief A process that outputs the differential entropy of the gaussian stored at every leave cell
// \author Isabel Restrepo
// \date 15-Nov-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <boxm/algo/boxm_merge_mog.h>


//:global variables
namespace boxm_compute_entropy_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool boxm_compute_entropy_process_cons(bprb_func_process& pro)
{
  using namespace boxm_compute_entropy_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool boxm_compute_entropy_process(bprb_func_process& pro)
{
  using namespace boxm_compute_entropy_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Scene is not valid!\n";
    return false;
  }

  //:Note initial implementation is for fixed types, but this can be changed if more cases are needed

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef boct_tree<short, gauss_type> gauss_tree_type;

  auto *gauss_scene = dynamic_cast<boxm_scene<gauss_tree_type>* > (scene_base.as_pointer());

  //check input's validity
  if (!gauss_scene) {
    std::cout <<  " :-- Scene is not of valid type!\n";
    return false;
  }

  //parameters and structure of the output scene are the same as those of the input scene
  boxm_scene<boct_tree<short, float> > *entropy_scene = new boxm_scene<boct_tree<short, float> >(gauss_scene->lvcs(), gauss_scene->origin(), gauss_scene->block_dim(), gauss_scene->world_dim(), gauss_scene->max_level(), gauss_scene->init_level());
  entropy_scene->set_paths(gauss_scene->path(), "entropy");
  entropy_scene->set_appearance_model(BOXM_FLOAT);

  compute_differential_entropy(*gauss_scene, *entropy_scene);
  gauss_scene->write_scene("entropy_scene.xml");
  pro.set_output_val<boxm_scene_base_sptr>(0, entropy_scene);
  std::cout<<"Exiting" << std::endl;

  return true;
}
