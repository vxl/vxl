//:
// \file
// \author Isabel Restrepo
// \date 21-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_pca_error_scenes.h>

//:global variables
namespace bvpl_load_pca_error_scenes_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_load_pca_error_scenes_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_pca_error_scenes_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i =0;
  input_types_[i++] = "boxm_scene_base_sptr";  //data scene
  input_types_[i++] = "vcl_string";   //pca path
  input_types_[i++] = "unsigned";   //dimension pca feture

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_pca_error_scenes_sptr" ;

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_load_pca_error_scenes_process(bprb_func_process& pro)
{
  using namespace bvpl_load_pca_error_scenes_process_globals;

  //get inputs
  unsigned i = 0;
  boxm_scene_base_sptr data_scene_base = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string pca_path = pro.get_input<std::string>(i++);
  auto feature_dim = pro.get_input<unsigned>(i++);

  if (!data_scene_base)
    return false;

  bvpl_pca_error_scenes *error_scenes = new bvpl_pca_error_scenes(data_scene_base, pca_path, feature_dim);
  //store output
  pro.set_output_val<bvpl_pca_error_scenes_sptr>(0, error_scenes);

  return true;
}
