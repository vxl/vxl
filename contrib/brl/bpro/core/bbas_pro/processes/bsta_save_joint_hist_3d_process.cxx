// This is brl/bpro/core/bbas_pro/processes/bsta_save_joint_hist_3d_process.cxx
#include <bprb/bprb_func_process.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <vsl/vsl_binary_io.h>
#include <bsta/io/bsta_io_histogram.h>
//:
// \file

//: Constructor
bool bsta_save_joint_hist_3d_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("bsta_joint_histogram_3d_base_sptr"); //joint_hist_3d
  input_types.push_back("vcl_string"); //joint_hist_3d path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  vcl_vector<vcl_string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool bsta_save_joint_hist_3d_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "bsta_save_joint_hist_3d_process: The input number should be 2" << vcl_endl;
    return false;
  }
  int i = 0;
  bsta_joint_histogram_3d_base_sptr jhist3d_ptr = pro.get_input<bsta_joint_histogram_3d_base_sptr>(i++);
  if(!jhist3d_ptr){
    vcl_cout << "in save_joint_hist_3d_process, null joint_histogram_3d pointer\n";
    return false;
  }
  
  vcl_string path = pro.get_input<vcl_string>(i++);
  vsl_b_ofstream os(path.c_str());
  if (!os) {
    vcl_cout << "in save_joint_hist_3d_process, couldn't open output file stream\n";
    return false;
  }
  vsl_b_write(os, jhist3d_ptr);
  return true;
}

