//:
// \file
// \brief A process to compute pca projection coeffiecients
// \author Isabel Restrepo
// \date 28-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_pca.h>
#include <bvpl/bvpl_octree/bvpl_scene_statistics.h>


//:global variables
namespace bvpl_pca_global_add_error_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_pca_global_add_error_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_pca_global_add_error_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvpl_global_pca_125_sptr" ; //global pca class
  input_types_[i++] = "int"; //scene id
  input_types_[i++] = "int";  //block Indices
  input_types_[i++] = "int";
  input_types_[i++] = "int";


  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_pca_global_add_error_process(bprb_func_process& pro)
{
  using namespace bvpl_pca_global_add_error_process_globals;

  //get inputs
  unsigned i = 0;
  bvpl_global_pca_125_sptr global_pca = pro.get_input<bvpl_global_pca_125_sptr>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);

  if (!global_pca)
  {
    std::cerr << "Global PCA is NULL\n";
    return false;
  }

  boxm_scene_base_sptr error_scene_base = global_pca->load_error_scene(scene_id);
  if (!error_scene_base) {
    std::cerr << "Error in bvpl_add_pca_errors_process: Error scene is of incorrect type\n";
    return false;
  }

  double error = bvpl_average_value(error_scene_base,block_i, block_j, block_k);
  std::cout << "Error at block: (" << block_i << ", " << block_j << ", " << block_k << ") is: " << error << '\n';

  //store output
    pro.set_output_val<double>(0, error);

  return true;
}
