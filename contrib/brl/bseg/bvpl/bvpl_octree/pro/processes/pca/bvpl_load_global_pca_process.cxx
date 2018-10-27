//:
// \file
// \brief
// \author Isabel Restrepo
// \date 15-May-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl_octree/bvpl_global_pca.h>

//: global variables
namespace bvpl_load_global_pca_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}


//: sets input and output types
bool bvpl_load_global_pca_125_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_global_pca_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_global_pca_125_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: the process
bool bvpl_load_global_pca_125_process(bprb_func_process& pro)
{
  using namespace bvpl_load_global_pca_process_globals;

  // get inputs
  std::string pca_file = pro.get_input<std::string>(0);
  auto *global_pca = new bvpl_global_pca<125>(pca_file);

  // store output
  pro.set_output_val<bvpl_global_pca_125_sptr>(0, global_pca);

  return true;
}
