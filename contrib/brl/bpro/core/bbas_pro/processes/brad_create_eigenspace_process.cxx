// This is brl/bpro/core/bbas_pro/processes/brad_create_eigenspace_process.cxx
#include <bprb/bprb_func_process.h>
#include <brad/brad_hist_prob_feature_vector.h>
#include <brad/brad_eigenspace.h>
//:
// \file

#include <vcl_fstream.h>

//: Constructor
bool brad_create_eigenspace_process_cons(bprb_func_process& pro)
{
  // no inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types(6);
  input_types[0]= "vcl_string"; // feature vector type
  input_types[1]= "float";      // hist min
  input_types[2]= "float";      // hist max
  input_types[3]= "unsigned";   // number of bins
  input_types[4]= "unsigned";   // number of cols in block - nib
  input_types[5]= "unsigned";   // number of cols in block - njb
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("brad_eigenspace_sptr"); //eigenspace
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;
  return true;
}

//: Execute the process
bool brad_create_eigenspace_process(bprb_func_process& pro)
{
#if 0
  // Sanity check
  if (pro.n_inputs()< 6) {
    vcl_cout << "brad_create_eigenspace_process: The input number should be 6" << vcl_endl;
    return false;
  }
#endif
  int i = 0;
  vcl_string feature_vector_type = pro.get_input<vcl_string>(i++);
  if(feature_vector_type != "brad_hist_prob_feature_vector"){
   vcl_cout << "brad_create_eigenspace_process: unknown feature vector type" << vcl_endl;
    return false;
  } 
  float min = pro.get_input<float>(i++);
  float max = pro.get_input<float>(i++);
  unsigned nbins = pro.get_input<unsigned>(i++);
  unsigned nib = pro.get_input<unsigned>(i++);
  unsigned njb = pro.get_input<unsigned>(i++);
  brad_hist_prob_feature_vector func(min, max, nbins);
  brad_eigenspace_sptr espace = 
    new brad_eigenspace<brad_hist_prob_feature_vector>(nib, njb, func);
  pro.set_output_val<brad_eigenspace_sptr>(0, espace);
  return true;
}

