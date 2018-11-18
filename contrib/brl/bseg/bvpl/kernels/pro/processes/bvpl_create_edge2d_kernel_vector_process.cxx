// This is brl/bseg/bvpl/kernels/pro/processes/bvpl_create_edge2d_kernel_vector_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for creating a planar edge kernel
// \author Isabel Restrepo
// \date June 24, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvpl/kernels/bvpl_edge2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>


namespace bvpl_create_edge2d_kernel_vector_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_create_edge2d_kernel_vector_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_edge2d_kernel_vector_process_globals;

  //process takes 2 inputs
  //input[0]: Kernel Height
  //input[1]: Kernel Width
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "unsigned";
  input_types_[1] = "unsigned";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_vector_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_create_edge2d_kernel_vector_process(bprb_func_process& pro)
{
  using namespace bvpl_create_edge2d_kernel_vector_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned int height = 5;
  height = pro.get_input<unsigned int>(0);
  unsigned int width = 5;
  width = pro.get_input<unsigned int>(1);


  //Create the factory and get the vector of kernels
  bvpl_edge2d_kernel_factory factory(height, width);
  bvpl_all_corner_dirs dir;
  bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);
  pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);

  return true;
}
