//:
// \file
// \brief A process to compute reconstruction error over all samples in a scene (test + train)
// \author Isabel Restrepo
// \date 13-Jan-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_discover_pca_kernels.h>
#include <bvpl/bvpl_octree/bvpl_pca_error_scenes.h>

//:global variables
namespace bvpl_compute_pca_error_block_process_globals
{
  constexpr unsigned n_inputs_ = 6;        //directory path, where pca_info.xml is
  constexpr unsigned n_outputs_ = 0;       //error file
}


//:sets input and output types
bool bvpl_compute_pca_error_block_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_pca_error_block_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvpl_discover_pca_kernels_sptr" ;  //directory path
  input_types_[i++] = "bvpl_pca_error_scenes_sptr";
  input_types_[i++] = "int";   //block Indices
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "unsigned";  //num_components


  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_compute_pca_error_block_process(bprb_func_process& pro)
{
  using namespace bvpl_compute_pca_error_block_process_globals;

  //get inputs
  unsigned i = 0;
  bvpl_discover_pca_kernels_sptr pca_extractor = pro.get_input<bvpl_discover_pca_kernels_sptr>(i++);
  bvpl_pca_error_scenes_sptr pca_error_scenes = pro.get_input<bvpl_pca_error_scenes_sptr>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  auto num_components = pro.get_input<unsigned>(i++);

  if (!pca_extractor)
    return false;

  if (!pca_error_scenes)
    return false;

  //bvpl_discover_pca_kernels pca_extractor(pca_dir);
  boxm_scene<boct_tree<short, float> >* error_scene = dynamic_cast<boxm_scene<boct_tree<short, float> >*> (pca_error_scenes->get_scene(num_components).as_pointer());
  pca_extractor->compute_testing_error(error_scene, num_components, block_i, block_j, block_k);

  return true;
}
