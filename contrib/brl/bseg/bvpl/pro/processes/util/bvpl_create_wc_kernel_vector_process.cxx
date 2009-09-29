// This is brl/bseg/bvpl/pro/processes/util/bvpl_create_wc_kernel_vector_process.cxx
#include <bvpl/bvpl_weighted_cube_kernel_factory.h>
//:
// \file
// \brief A class for creating a vector of 3D cube kernels
//
// \author Isabel Restrepo
// \date September 29, 2009
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvpl/bvpl_create_directions.h>


namespace bvpl_create_wc_kernel_vector_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}


bool bvpl_create_wc_kernel_vector_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_wc_kernel_vector_process_globals;
  //process takes 3inputs
  //input[0]: Kernel half length
  //input[1]: Kernel half width
  //input[2]: Kernel half thickness

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "unsigned";
  input_types_[1] = "unsigned";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";

  if (!pro.set_input_types(input_types_))
    return false;

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_vector_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool bvpl_create_wc_kernel_vector_process(bprb_func_process& pro)
{
  using namespace bvpl_create_wc_kernel_vector_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned int length = 5;
  length = pro.get_input<unsigned int>(0);
  unsigned int width = 5;
  width = pro.get_input<unsigned int>(1);
  unsigned int thickness = 5;
  thickness = pro.get_input<unsigned int>(2);
  vcl_string dir_type = pro.get_input<vcl_string>(3);

  //Create the factory and get the vector of kernels
  bvpl_weighted_cube_kernel_factory factory(length,width,thickness);
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

  return true;
}
