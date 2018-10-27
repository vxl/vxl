//:
// \file
// \brief A process to compute the Taylor reconstruction error of a block (Assumes, basis has been applied)
// \author Isabel Restrepo
// \date 9-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_taylor_basis.h>
#include <boxm/boxm_scene.h>

//:global variables
namespace bvpl_compute_taylor_error_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_compute_taylor_error_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_taylor_error_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr"; //the original data scene
  input_types_[i++] = "bvpl_taylor_scenes_map_sptr" ; //scenes needed for Taylor operations
  input_types_[i++] = "int" ; //block index in x-dimension
  input_types_[i++] = "int" ; //block index in y-dimension
  input_types_[i++] = "int" ; //block index in z-dimension

  std::vector<std::string> output_types_(n_outputs_);
  i=0;
  output_types_[i++] = "boxm_scene_base_sptr"; //the error for this block
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_compute_taylor_error_process(bprb_func_process& pro)
{
  using namespace bvpl_compute_taylor_error_process_globals;

  //get inputs
  unsigned i = 0;
  boxm_scene_base_sptr data_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  bvpl_taylor_scenes_map_sptr taylor_scenes = pro.get_input<bvpl_taylor_scenes_map_sptr>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  if (!data_scene_base) {
    std::cerr << "Error in bvpl_compute_taylor_error_process: Null data scene\n";
    return false;
  }
  if (!taylor_scenes) {
    std::cerr << "Error in bvpl_compute_taylor_error_process: Null scenes map\n";
    return false;
  }

  //assemble the Taylor basis
  bvpl_taylor_basis::assemble_basis(taylor_scenes, block_i, block_j, block_k);

  //compute the error block
  boxm_scene<boct_tree<short, float> >* error_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (taylor_scenes->get_scene("error").as_pointer());
  auto* data_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (data_scene_base.as_pointer());
  double cell_length = data_scene->finest_cell_length();
  bvpl_taylor_basis::compute_approximation_error(data_scene, taylor_scenes->get_scene("basis"),
                                                 error_scene,taylor_scenes->loader(),
                                                 block_i, block_j, block_k, cell_length);

  pro.set_output_val<boxm_scene_base_sptr>(0, error_scene);

  return true;
}
