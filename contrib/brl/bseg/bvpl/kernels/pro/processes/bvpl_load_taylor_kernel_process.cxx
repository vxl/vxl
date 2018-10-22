//:
// \file
// \brief A process to load a taylor kernel from file
// \author Isabel Restrepo
// \date 1-Feb-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/kernels/bvpl_taylor_basis_factory.h>

//:global variables
namespace bvpl_load_taylor_kernel_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}


//:sets input and output types
bool bvpl_load_taylor_kernel_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_taylor_kernel_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_load_taylor_kernel_process(bprb_func_process& pro)
{
  using namespace bvpl_load_taylor_kernel_process_globals;

  //get inputs
  std::string filename = pro.get_input<std::string>(0);

  std::cout << "Reading kernel file : " << filename << std::endl;

  bvpl_taylor_basis_factory factory(filename);

  bvpl_kernel_sptr kernel = new bvpl_kernel(factory.create());

  //store output
  pro.set_output_val<bvpl_kernel_sptr>(0, kernel);

  return true;
}
