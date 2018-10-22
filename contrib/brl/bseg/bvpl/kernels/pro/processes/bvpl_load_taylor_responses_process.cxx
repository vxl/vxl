//:
// \file
// \brief A process to load taylor response scenes into memory
// \author Isabel Restrepo
// \date 9-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/kernels/bvpl_taylor_basis_factory.h>
#include <bvpl/bvpl_octree/bvpl_taylor_basis.h>

//:global variables
namespace bvpl_load_taylor_responses_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_load_taylor_responses_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_taylor_responses_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; //parent dir of responses

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_taylor_scenes_map_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_load_taylor_responses_process(bprb_func_process& pro)
{
  using namespace bvpl_load_taylor_responses_process_globals;

  //get inputs
  std::string parent_dir = pro.get_input<std::string>(0);

  bvpl_taylor_basis_loader loader(parent_dir);
  bvpl_taylor_scenes_map_sptr  taylor_scenes = new bvpl_taylor_scenes_map(loader);

  //store output
  pro.set_output_val<bvpl_taylor_scenes_map_sptr>(0, taylor_scenes);

  return true;
}
