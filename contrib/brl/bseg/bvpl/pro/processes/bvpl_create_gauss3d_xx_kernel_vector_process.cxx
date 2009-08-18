// This is brl/bseg/bvpl/pro/processes/bvpl_create_gauss3d_xx_kernel_vector_process.cxx

//:
// \file
// \brief A class for creating a vector of 3D gaussian second derivative kernels
//
// \author Isabel Restrepo
// \date 6/29/09
// \verbatim
//  Modifications
// \endverbatim 

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_gauss3d_xx_kernel_factory.h>



namespace bvpl_create_gauss3d_xx_kernel_vector_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 1;
}


bool bvpl_create_gauss3d_xx_kernel_vector_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_gauss3d_xx_kernel_vector_process_globals;
  //process takes 3inputs
  //input[0]: Kernel sigma1
  //input[1]: Kernel sigma2
  //input[2]: Kernel sigma3
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "float";
  input_types_[1] = "float";
  input_types_[2] = "float";
  
  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_vector_sptr";
  if (!pro.set_output_types(output_types_))
    return false;
  
  return true;
}

bool bvpl_create_gauss3d_xx_kernel_vector_process(bprb_func_process& pro)
{
  using namespace bvpl_create_gauss3d_xx_kernel_vector_process_globals;
  
  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  
  //get inputs:
  float sigma1 = 1.5;
  sigma1 = pro.get_input<float>(0);
  float sigma2 = 1;
  sigma2 = pro.get_input<float>(1);
  float sigma3 = 1;
  sigma3 = pro.get_input<float>(2);
  
  
  //Create the factory and get the vector of kernels
  bvpl_gauss3d_xx_kernel_factory factory(sigma1,sigma2,sigma3);
  bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector();
  pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);
  
  
  return true;
}

