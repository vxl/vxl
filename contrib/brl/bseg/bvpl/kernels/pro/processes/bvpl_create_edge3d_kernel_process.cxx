// This is brl/bseg/bvpl/kernels/pro/processes/bvpl_create_edge3d_kernel_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for creating a volume edge kernel
// \author Vishal Jain
// \date June 26, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvpl/kernels/bvpl_edge3d_kernel_factory.h>


namespace bvpl_create_edge3d_kernel_process_globals
{
  //the specs of this kernel are specified as parameters
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_create_edge3d_kernel_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_edge3d_kernel_process_globals;

  //process takes 10 inputs
  //input[0]-input[5]: Min and max coordinates of kernel
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "int";
  input_types_[1] = "int";
  input_types_[2] = "int";
  input_types_[3] = "int";
  input_types_[4] = "int";
  input_types_[5] = "int";
  input_types_[6] = "float";
  input_types_[7] = "float";
  input_types_[8] = "float";
  input_types_[9] = "float";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvpl_kernel_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_create_edge3d_kernel_process(bprb_func_process& pro)
{
  using namespace bvpl_create_edge3d_kernel_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  int min_x = pro.get_input<int>(i++);
  int max_x = pro.get_input<int>(i++);
  int min_y = pro.get_input<int>(i++);
  int max_y = pro.get_input<int>(i++);
  int min_z = pro.get_input<int>(i++);
  int max_z = pro.get_input<int>(i++);

  auto axis_x = pro.get_input<float>(i++);
  auto axis_y = pro.get_input<float>(i++);
  auto axis_z = pro.get_input<float>(i++);
  auto angle = pro.get_input<float>(i++);

  vnl_float_3 axis(axis_x,axis_y, axis_z);

  //Create the factory
  bvpl_edge3d_kernel_factory factory(min_x, max_x,min_y,max_y,min_z,max_z);
  factory.set_rotation_axis(axis);
  factory.set_angle(angle);

  bvpl_kernel_sptr kernel_sptr = new bvpl_kernel(factory.create());
  kernel_sptr->print();
  kernel_sptr->save_raw("edge_kernel.raw");
  pro.set_output_val<bvpl_kernel_sptr>(0, kernel_sptr);

  return true;
}
