//:
// \file
// \brief A process to init auxiliary scenes and other variables
// \author Isabel Restrepo
// \date 12-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

//:global variables
namespace bvpl_init_global_taylor_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_init_global_taylor_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_init_global_taylor_process_globals ;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // path to taylor_global_info file

  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_init_global_taylor_process(bprb_func_process& pro)
{
  using namespace bvpl_init_global_taylor_process_globals;

  //get inputs
  vcl_string taylor_dir = pro.get_input<vcl_string>(0);

  bvpl_global_taylor global_taylor(taylor_dir);
  global_taylor.init();

  return true;
}
