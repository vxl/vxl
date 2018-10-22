//:
// \file
// \author Isabel Restrepo
// \date 12-Apr-2011

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvpl/bvpl_octree/bvpl_global_taylor.h>

#include <vul/vul_file.h>

//:global variables
namespace bvpl_compute_taylor_coefficients_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 0;
}


//:sets input and output types
bool bvpl_compute_taylor_coefficients_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compute_taylor_coefficients_process_globals ;

  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "vcl_string" ;  //path of taylor_global_info.xml
  input_types_[i++] = "int"; //scene id
  input_types_[i++] = "int";  //block Indices
  input_types_[i++] = "int";
  input_types_[i++] = "int";
  input_types_[i++] = "int"; // dimension (3 for xyz derivatives, 10 for all 1st and 2nd order derivatives)

  std::vector<std::string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool bvpl_compute_taylor_coefficients_process(bprb_func_process& pro)
{
  using namespace bvpl_compute_taylor_coefficients_process_globals;

  //get inputs
  unsigned i = 0;
  std::string taylor_dir = pro.get_input<std::string>(i++);
  int scene_id = pro.get_input<int>(i++);
  int block_i = pro.get_input<int>(i++);
  int block_j = pro.get_input<int>(i++);
  int block_k = pro.get_input<int>(i++);
  int dim = pro.get_input<int>(i++);

  if (!vul_file::is_directory(taylor_dir))
    return false;

  switch (dim) {
    case 3:
    {
      std::string kernel_names[] = {"Ix", "Iy", "Iz"};
      bvpl_global_taylor<float, 3> global_taylor(taylor_dir, kernel_names);
      global_taylor.compute_taylor_coefficients(scene_id, block_i, block_j, block_k);
      break;
    }
    case 10:
    {
      const std::string kernel_names[10] = {"I0", "Ix", "Iy", "Iz", "Ixx", "Iyy", "Izz", "Ixy", "Ixz", "Iyz" };
      bvpl_global_taylor<double, 10> global_taylor(taylor_dir, kernel_names);
      global_taylor.compute_taylor_coefficients(scene_id, block_i, block_j, block_k);
      break;
    }
    default:
      break;
  }

  return true;
}
