//:
// \file
// \brief A process to init auxiliary scenes and other variables
// \author Isabel Restrepo
// \date 12-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

//:global variables
namespace bvpl_init_global_taylor_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_init_global_taylor_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_init_global_taylor_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string"; // path to taylor_global_info file
  input_types_[1] = "int"; // dimension (3 for xyz derivatives, 10 for all 1st and 2nd order derivatives)

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_init_global_taylor_process(bprb_func_process& pro)
{
  using namespace bvpl_init_global_taylor_process_globals;

  //get inputs
  std::string taylor_dir = pro.get_input<std::string>(0);
  int dim = pro.get_input<int>(1);

  switch (dim) {
    case 3:
    {
      std::string kernel_names[] = {"Ix", "Iy", "Iz"};
      bvpl_global_taylor<float, 3> global_taylor(taylor_dir, kernel_names);
      global_taylor.init();
      break;
    }
    case 10:
    {
      const std::string kernel_names[10] = {"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz" };
      bvpl_global_taylor<double, 10> global_taylor(taylor_dir, kernel_names);
      global_taylor.init();
      break;
    }
    default:
      break;
  }



  return true;
}
