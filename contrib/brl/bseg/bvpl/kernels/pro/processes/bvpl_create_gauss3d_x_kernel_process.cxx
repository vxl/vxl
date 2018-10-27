//:
// \file
// \author Isabel Restrepo
// \date 11-Oct-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vnl/vnl_float_3.h>
#include <bvpl/kernels/bvpl_gauss3d_x_kernel_factory.h>

//: global variables
namespace bvpl_create_gauss3d_x_kernel_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 1;
}


//: sets input and output types
bool bvpl_create_gauss3d_x_kernel_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_gauss3d_x_kernel_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  for (unsigned i = 0; i<n_inputs_; i++)
    input_types_[i] ="float" ;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_kernel_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_create_gauss3d_x_kernel_process(bprb_func_process& pro)
{
  using namespace bvpl_create_gauss3d_x_kernel_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << " The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs
  unsigned i = 0;
  auto sigma1 = pro.get_input<float>(i++); //sigma1 - x-axis width
  auto sigma2 = pro.get_input<float>(i++); //sigma2 - y-axis width
  auto sigma3 = pro.get_input<float>(i++); //sigma3 - z-axis width
  auto axis_x = pro.get_input<float>(i++);
  auto axis_y = pro.get_input<float>(i++);
  auto axis_z = pro.get_input<float>(i++);
  auto angle =  pro.get_input<float>(i++);

  vnl_float_3 axis(axis_x, axis_y, axis_z);

  //Create the factory
  bvpl_gauss3d_x_kernel_factory factory(sigma1, sigma2, sigma3);
  factory.set_rotation_axis(axis);
  factory.set_angle(angle);

  bvpl_kernel_sptr kernel_sptr = new bvpl_kernel(factory.create());
  kernel_sptr->print();
  pro.set_output_val<bvpl_kernel_sptr>(0, kernel_sptr);

  return true;
}
