//:
// \file
// \brief A process to init auxiliary scenes and other variables
// \author Isabel Restrepo
// \date 23-Mar-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_pca.h>

//:global variables
namespace bvpl_init_global_pca_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_init_global_pca_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_init_global_pca_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // path to pca_global_info file
  input_types_[1] = "unsigned"; //id of the scene to initialize

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_init_global_pca_process(bprb_func_process& pro)
{
  using namespace bvpl_init_global_pca_process_globals;

  //get inputs
  std::string pca_dir = pro.get_input<std::string>(0);
  auto scene_id = pro.get_input<unsigned>(1);

  bvpl_global_pca<125> global_pca(pca_dir);
  global_pca.init(scene_id);

  return true;
}
