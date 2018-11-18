// This is brl/bseg/bvpl/kernels/pro/processes/bvpl_create_corner2d_kernel_vector_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for creating a vector of 2d-corner kernels
// \author Isabel Restrepo
// \date August 26, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvpl/kernels/bvpl_corner2d_kernel_factory.h>
#include <bvpl/kernels/bvpl_create_directions.h>


namespace bvpl_create_corner2d_kernel_vector_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_create_corner2d_kernel_vector_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_corner2d_kernel_vector_process_globals;

  //process takes 4 inputs
  //input[0]: Kernel length
  //input[1]: Kernel width
  //input[2]: Kernel thickness
  //input[3]: String : type of kernel directions
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "unsigned";
  input_types_[1] = "unsigned";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_vector_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_create_corner2d_kernel_vector_process(bprb_func_process& pro)
{
  using namespace bvpl_create_corner2d_kernel_vector_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned int length = 5;
  length = pro.get_input<unsigned int>(0);
  unsigned int width = 5;
  width = pro.get_input<unsigned int>(1);
  unsigned int thickness = 5;
  thickness = pro.get_input<unsigned int>(2);
  std::string dir_type =pro.get_input<std::string>(3);

  //Create the factory and get the vector of kernels
  bvpl_corner2d_kernel_factory factory(length,width,thickness);

  if (dir_type == "main_corners") {
    bvpl_main_corner_dirs dir;
    bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);
    pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);
  }
  if (dir_type == "main_plane") {
    bvpl_main_plane_corner_dirs dir;
    bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);
    pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);
  }
  if (dir_type == "all_corners") {
    bvpl_all_corner_dirs dir;
    bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);
    pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);
  }
  if (dir_type == "pi_over_2_corners") {
    bvpl_pi_over_2_corner_dirs dir;
    bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);
    pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);
  }

  return true;
}
