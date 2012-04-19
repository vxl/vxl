// This is brl/bseg/bvpl/kernels/pro/processes/bvpl_create_generic_kernel_vector_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for creating generic kernel vectors
// \author Isabel Restrepo
// \date August 26, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvpl/kernels/bvpl_create_directions.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>


namespace bvpl_create_generic_kernel_vector_process_globals
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 1;
}


bool bvpl_create_generic_kernel_vector_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_generic_kernel_vector_process_globals;

  //process takes 4 inputs
  //input[0]: Kernel length
  //input[1]: Kernel width
  //input[2]: Kernel thickness
  //input[3]: Kernel support_x 
  //input[4]: Kernel support_y
  //input[5]: Kernel support_z
  //input[6]: String : type of kernel 
  //input[4]: String : type of directions
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "float";
  input_types_[1] = "float";
  input_types_[2] = "float";
  input_types_[3] = "float";
  input_types_[4] = "float";
  input_types_[5] = "float";
  input_types_[6] = "vcl_string";
  input_types_[7] = "vcl_string";


  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_vector_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_create_generic_kernel_vector_process(bprb_func_process& pro)
{
  using namespace bvpl_create_generic_kernel_vector_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  float dim_x = 1.0f; dim_x = pro.get_input<float>(0);
  float dim_y = 1.0f; dim_y = pro.get_input<float>(1);
  float dim_z = 1.0f; dim_z = pro.get_input<float>(2);
  float supp_x = 1.0f; supp_x = pro.get_input<float>(3);
  float supp_y = 1.0f; supp_y = pro.get_input<float>(4);
  float supp_z = 1.0f; supp_z = pro.get_input<float>(5);
  vcl_string factory_name =pro.get_input<vcl_string>(6);
  vcl_string dir_type =pro.get_input<vcl_string>(7);


  if (factory_name == "gauss_x") //first order derivative of gaussian
  {
    //Create the factory and get the vector of kernels
    bvpl_gauss3d_x_kernel_factory factory(dim_x, dim_y, dim_z, supp_x, supp_y, supp_z);  //in this case the values of dim_* correspond of sigma
    if (dir_type == "XYZ") {
      bvpl_create_directions_xyz dir;
      bvpl_kernel_vector_sptr kernels = factory.create_kernel_vector(dir);
      pro.set_output_val<bvpl_kernel_vector_sptr>(0, kernels);
      return true;
    }
    
  }

  //if got to here is because we didn't meet available conditions
  vcl_cerr << "Error: No vector could be created for specified factory and directions " <<vcl_endl;
  pro.set_output_val<bvpl_kernel_vector_sptr>(0, NULL);
  return false;
}

