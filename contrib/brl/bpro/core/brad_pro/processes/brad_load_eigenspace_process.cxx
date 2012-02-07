// This is brl/bpro/core/brad_pro/processes/brad_load_eigenspace_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_eigenspace.h>
#include <vsl/vsl_binary_io.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_load_eigenspace_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); //eigenspace path
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //no output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("brad_eigenspace_sptr"); //eigenspace ptr
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_load_eigenspace_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs()< 1) {
    vcl_cout << "brad_load_eigenspace_process: The input number should be 1" << vcl_endl;
    return false;
  }
  int i = 0;
  vcl_string path = pro.get_input<vcl_string>(i++);
  vsl_b_ifstream is(path.c_str());
  if (!is) {
    vcl_cout << "in load_eigenspace_process, couldn't open input file stream\n";
    return false;
  }
  brad_eigenspace_sptr espace;
  vsl_b_read(is, espace);
  if (!espace){
    vcl_cout << "in load_eigenspace_process, null eigenspace pointer\n";
    return false;
  }
  pro.set_output_val<brad_eigenspace_sptr>(0, espace);
  return true;
}

