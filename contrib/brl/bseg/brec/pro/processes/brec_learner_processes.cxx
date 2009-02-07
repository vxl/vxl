// This is brl/bseg/brec/pro/processes/brec_learner_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to initialize learner class instances, collect stats and fit parametric distributions
//
// \author Ozge Can Ozcanli
// \date Jan 22, 2009
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <brec/brec_part_hierarchy_learner_sptr.h>
#include <brec/brec_part_hierarchy_learner.h>
#include <brec/brec_part_hierarchy.h>

//: Constructor
bool brec_learner_layer0_init_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("int");     // number of directions to try in range [180,-180)
  input_types.push_back("float");   // lambda range (e.g. will try lambda0 and lambda1 in [1.0, lambda range])
  input_types.push_back("float");   // lambda increment (e.g. will increment lambda0 and lambda1 with this amount)
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("brec_part_hierarchy_learner_sptr"); // learner instance, initialized according to the input params
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer0_init_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    vcl_cerr << " brec_learner_init_layer0_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  int ndirs = pro.get_input<int>(i++);
  float lambda_range = pro.get_input<float>(i++);
  float lambda_inc = pro.get_input<float>(i++);

  brec_part_hierarchy_learner_sptr hl = new brec_part_hierarchy_learner();
  hl->initialize_layer0_as_gaussians(ndirs, lambda_range, lambda_inc);

  pro.set_output_val<brec_part_hierarchy_learner_sptr>(0, hl);

  return true;
}


//: Constructor
bool brec_learner_layer0_fit_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brec_part_hierarchy_learner_sptr");     // the already initialized learner instance
  input_types.push_back("vcl_string");     // the name of output file to print fitted distributions
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("brec_part_hierarchy_learner_sptr"); // return learner instance back
  ok = pro.set_output_types(output_types);
  return ok;
}


bool brec_learner_layer0_fit_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << " brec_learner_fit_layer0_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);
  vcl_string output_name = pro.get_input<vcl_string>(i++);

  hl->layer0_fit_parametric_dist();
  hl->print_to_m_file_layer0_fitted_dists(output_name);

  pro.set_output_val<brec_part_hierarchy_learner_sptr>(0, hl);

  return true;
}

//: Constructor
bool brec_learner_layer0_rank_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  vcl_vector<vcl_string> input_types;
  input_types.push_back("brec_part_hierarchy_learner_sptr");     // the already initialized learner instance
  input_types.push_back("int");  // N: top N layer0 prims are used in the created hierarchy
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  vcl_vector<vcl_string> output_types;
  output_types.push_back("brec_part_hierarchy_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer0_rank_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    vcl_cerr << " brec_learner_layer0_rank_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);
  int N = pro.get_input<int>(i++);

  brec_part_hierarchy_sptr h = hl->layer0_rank_and_create_hierarchy(N);
  pro.set_output_val<brec_part_hierarchy_sptr>(0, h);

  return true;
}

