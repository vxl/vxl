// This is brl/bpro/core/bbas_pro/processes/bsta_clone_joint_hist_3d_process.cxx
#include <bprb/bprb_func_process.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <bsta/io/bsta_io_histogram.h>
#include <vsl/vsl_binary_io.h>
//:
// \file

//: Constructor
bool bsta_clone_joint_hist_3d_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bsta_joint_histogram_3d_base_sptr"); //joint_hist_3d
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
bool bsta_clone_joint_hist_3d_process(bprb_func_process& pro)
{

  // Sanity check
  if (pro.n_inputs()< 1) {
    std::cout << "bsta_clone_joint_hist_3d_process: The input number should be 1" << std::endl;
    return false;
  }

  bsta_joint_histogram_3d_base_sptr in_hist_ptr =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(0);
  if (!in_hist_ptr) {
    std::cout << "in clone_joint_hist_3d_process, null input hist ptr\n";
    return false;
  }
  auto* h = dynamic_cast<bsta_joint_histogram_3d<float>*>(in_hist_ptr.ptr());

  auto* h_clone  =
    new bsta_joint_histogram_3d<float>(*h);
  h_clone->clear();

  bsta_joint_histogram_3d_base_sptr out_ptr = h_clone;
  pro.set_output_val<bsta_joint_histogram_3d_base_sptr>(0, out_ptr);

  return true;
}
