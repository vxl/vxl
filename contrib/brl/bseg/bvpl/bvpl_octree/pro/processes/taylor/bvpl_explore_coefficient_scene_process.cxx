//:
// \file
// \brief A process to extract a Taylor coefficient from the 10-d vector
// \author Isabel Restrepo
// \date 12-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

//:global variables
namespace bvpl_explore_coefficient_scene_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_explore_coefficient_scene_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_explore_coefficient_scene_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string"; //path to Taylor info file
  input_types_[i++] = "int"; //scene id
  input_types_[i++] = "int"; //coefficient

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_explore_coefficient_scene_process(bprb_func_process& pro)
{
  using namespace bvpl_explore_coefficient_scene_process_globals;

  //get inputs
  std::string taylor_dir = pro.get_input<std::string>(0);
  int scene_id = pro.get_input<int>(1);
  int coeff_id = pro.get_input<int>(2);

  const std::string kernel_names[10] = {"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz" };
  bvpl_global_taylor<double, 10> taylor(taylor_dir, kernel_names);

  boxm_scene_base_sptr valid_scene_base = taylor.load_valid_scene(scene_id);
  auto* valid_scene = dynamic_cast<boxm_scene<boct_tree<short, bool> >*> (valid_scene_base.as_pointer());
  if (!valid_scene) {
    std::cerr << "Error in bvpl_explore_coefficient_scene_process: Could not cast valid scene\n";
    return false;
  }

  std::stringstream scene_ss;
  scene_ss << "coefficient_" << coeff_id << "_scene_" << scene_id ;
  std::string scene_path = taylor.aux_dirs(scene_id) + "/" + scene_ss.str() + ".xml";

  std::cout << "Initializing " << scene_path << '\n'
           << "Scene: " << scene_path << " does not exist, initializing xml" << std::endl;
  boxm_scene<boct_tree<short, float> > *scene =
  new boxm_scene<boct_tree<short, float> >(valid_scene->lvcs(), valid_scene->origin(), valid_scene->block_dim(), valid_scene->world_dim(), valid_scene->max_level(), valid_scene->init_level());
  scene->set_appearance_model(BOXM_FLOAT);
  scene->set_paths(taylor.aux_dirs(scene_id), scene_ss.str());
  scene->write_scene("/" + scene_ss.str() +  ".xml");
  taylor.extract_coefficient_scene(scene_id,  coeff_id, scene);

  //store output
  pro.set_output_val<boxm_scene_base_sptr>(0, scene);

  return true;
}
