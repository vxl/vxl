// This is brl/bpro/core/bbas_pro/processes/bsta_load_joint_hist_3d_process.cxx
#include <bprb/bprb_func_process.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <bsta/io/bsta_io_histogram.h>
#include <vsl/vsl_binary_io.h>
//:
// \file

//: Constructor
bool bsta_load_joint_hist_3d_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); //joint_hist_3d path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  std::vector<std::string> output_types;
  output_types.emplace_back("bsta_joint_histogram_3d_base_sptr"); //joint_hist_3d
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool bsta_load_joint_hist_3d_process(bprb_func_process& pro)
{

  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "bsta_load_joint_hist_3d_process: The input number should be 1" << std::endl;
    return false;
  }

  std::string path = pro.get_input<std::string>(0);
  vsl_b_ifstream is(path.c_str());
  if (!is) {
    std::cout << "in load_joint_hist_3d_process, couldn't open input file stream\n";
    return false;
  }

  bsta_joint_histogram_3d_base_sptr jh3d_ptr;
  vsl_b_read(is, jh3d_ptr);

  pro.set_output_val<bsta_joint_histogram_3d_base_sptr>(0, jh3d_ptr);

  return true;
}
