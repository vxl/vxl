//:
// \file
// \brief A process to compute taylor projection coeffiecients
// \author Isabel Restrepo
// \date 28-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

//:global variables
namespace bvpl_taylor_global_proj_error_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_taylor_global_proj_error_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_taylor_global_proj_error_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvpl_global_taylor_sptr" ; //global taylor class
  input_types_[i++] = "int"; //scene id
  input_types_[i++] = "int";  //block Indices
  input_types_[i++] = "int";
  input_types_[i++] = "int";

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_taylor_global_proj_error_process(bprb_func_process& pro)
{
  using namespace bvpl_taylor_global_proj_error_process_globals;

  //get inputs
  unsigned i = 0;
  bvpl_global_taylor_sptr global_taylor = pro.get_input<bvpl_global_taylor_sptr>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);

  if (!global_taylor)
  {
    std::cerr << "Global taylor is NULL\n";
    return false;
  }

  global_taylor->compute_approximation_error(scene_id, block_i, block_j, block_k);

  return true;
}
