//:
// \file
// \brief A process to add Taylor error at a percentage of voxels in a block
// \author Isabel Restrepo
// \date 15-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_pca_error_scenes.h>
#include <bvpl/bvpl_octree/bvpl_scene_statistics.h>

#include <boxm/boxm_scene.h>

//:global variables
namespace bvpl_add_pca_errors_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_add_pca_errors_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_add_pca_errors_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvpl_pca_error_scenes_sptr";
  input_types_[i++] = "unsigned"; //dimension
  input_types_[i++] = "double";  ////fraction [0,1] of cells used in the computation
  input_types_[i++] = "int" ; //block index in x-dimension
  input_types_[i++] = "int" ; //block index in y-dimension
  input_types_[i++] = "int" ; //block index in z-dimension

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_add_pca_errors_process(bprb_func_process& pro)
{
  using namespace bvpl_add_pca_errors_process_globals;

  //get inputs
  unsigned i =0;
  bvpl_pca_error_scenes_sptr error_scenes = pro.get_input<bvpl_pca_error_scenes_sptr>(i++);
  auto dim = pro.get_input<unsigned>(i++);
  auto fraction_nsamples = pro.get_input<double>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);

  if (!error_scenes) {
    std::cerr << "Error in bvpl_add_pca_errors_process: Null error scene\n";
    return false;
  }
  boxm_scene<boct_tree<short, float> >* error_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (error_scenes->get_scene(dim).as_pointer());
  if (!error_scene) {
    std::cerr << "Error in bvpl_add_pca_errors_process: Error scene is of incorrect type\n";
    return false;
  }
  //sum errors within block
  double error = 0;
  if (fraction_nsamples < 0.95)
  {
    auto scene_ncells = (double)error_scene->size();
    error_scene->load_block(block_i,block_j,block_k);
    auto tree_ncells = (double)error_scene->get_block(block_i,block_j,block_k)->get_tree()->size();
    double nsamples = scene_ncells * fraction_nsamples;

    auto tree_nsamples = (unsigned long)((tree_ncells/scene_ncells)*nsamples);
    std::cout << "Number of samples in  the scene " << scene_ncells << '\n'
             << "Adding errors from " << tree_nsamples << " samples in block: " << block_i << ',' << block_j << ',' << block_k << '\n';
    error = bvpl_average_value(error_scene,block_i, block_j, block_k, tree_nsamples);
    std::cout << "Error at block: (" << block_i << ", " << block_j << ", " << block_k << ") and dim: " << dim << "is: " << error << '\n';
  }
  else
  {
    error = bvpl_average_value(error_scene,block_i, block_j, block_k);
    std::cout << "Error at block: (" << block_i << ", " << block_j << ", " << block_k << ") and dim: " << dim << "is: " << error << '\n';
  }

  //store output
  pro.set_output_val<double>(0, error);

  return true;
}
