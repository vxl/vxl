// This is brl/bpro/core/bbas_pro/processes/bsta_joint_hist_3d_vrml_process.cxx
#include <bprb/bprb_func_process.h>
#include <bsta/bsta_joint_histogram_3d.h>
#include <vsl/vsl_binary_io.h>
//:
// \file

//: Constructor
bool bsta_joint_hist_3d_vrml_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bsta_joint_histogram_3d_base_sptr"); //joint_hist_3d
  input_types.emplace_back("vcl_string"); //vrml path
  input_types.emplace_back("bool"); //relative probability
  input_types.emplace_back("float");// red
  input_types.emplace_back("float");// green
  input_types.emplace_back("float");// blue
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool bsta_joint_hist_3d_vrml_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 6) {
    std::cout << "bsta_joint_hist_3d_vrml_process: The input number should be 6" << std::endl;
    return false;
  }
  int i = 0;

  bsta_joint_histogram_3d_base_sptr jhist3d =
    pro.get_input<bsta_joint_histogram_3d_base_sptr>(i++);

  if(!jhist3d){
    std::cout << "bsta_joint_hist_3d_vrml_process: null histogram pointer\n";
    return false;
  }

  std::string path = pro.get_input<std::string>(i++);
  std::ofstream os(path.c_str());
  if(!os.is_open()){
    std::cout << "bsta_joint_hist_3d_vrml_process: can't open output stream\n";
    return false;
  }
  bool relative_prob = pro.get_input<bool>(i++);
  auto r = pro.get_input<float>(i++);
  auto g = pro.get_input<float>(i++);
  auto b = pro.get_input<float>(i);
  //for now only a float type is used
  auto* h3df =
    dynamic_cast<bsta_joint_histogram_3d<float>*>(jhist3d.ptr());
  h3df->print_to_vrml(os, relative_prob, r, g, b);
  return true;
}
