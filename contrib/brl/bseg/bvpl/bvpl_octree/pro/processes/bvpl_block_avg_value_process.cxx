//:
// \file
// \brief A process to add taylor error at a percentage of voxels in a block
// \author Isabel Restrepo
// \date 15-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_scene_statistics.h>
#include <boxm/boxm_scene.h>

//:global variables
namespace bvpl_block_avg_value_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_block_avg_value_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_block_avg_value_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "boxm_scene_base_sptr";
  input_types_[i++] = "double";  ////fraction [0,1] of cells used in the computation
  input_types_[i++] = "int" ; //block index in x-dimension
  input_types_[i++] = "int" ; //block index in y-dimension
  input_types_[i++] = "int" ; //block index in z-dimension

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "double";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_block_avg_value_process(bprb_func_process& pro)
{
  using namespace bvpl_block_avg_value_process_globals;

  //get inputs
  unsigned i =0;
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  double fraction_nsamples = pro.get_input<double>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);

  if (!scene_base){
    std::cerr << "Error in bvpl_block_avg_value_process: Null error scene\n";
    return false;
  }
  boxm_scene<boct_tree<short, float> >* scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (scene_base.as_pointer());
  if (!scene){
    std::cerr << "Error in bvpl_block_avg_value_process: Error scene is of incorrect type\n";
    return false;
  }
  //sum errors within block
  double scene_ncells = (double)scene->size();
  scene->load_block(block_i,block_j,block_k);
  double tree_ncells = (double)scene->get_block(block_i,block_j,block_k)->get_tree()->size();
  double nsamples = scene_ncells * fraction_nsamples;

  //number of samples - 10% of total number of leaf-cells
  unsigned long tree_nsamples = (unsigned long)((tree_ncells/scene_ncells)*nsamples);
  std::cout << "Number of samples in  the scene " << scene_ncells << '\n'
           << "Adding errors from " << tree_nsamples << " samples in block: " << block_i << ',' << block_j << ',' << block_k << std::endl;
  double avg_value = bvpl_average_value(scene,block_i, block_j, block_k, tree_nsamples);

  //store output
  pro.set_output_val<double>(0, avg_value);

  return true;
}
