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
  std::vector<std::string> input_types;
  input_types.emplace_back("int");     // number of directions to try in range [180,-180)
  input_types.emplace_back("float");   // lambda range (e.g. will try lambda0 and lambda1 in [1.0, lambda range])
  input_types.emplace_back("float");   // lambda increment (e.g. will increment lambda0 and lambda1 with this amount)
  input_types.emplace_back("int");  // visualization parameter n, n*n histograms are plotted in a row into the m file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_learner_sptr"); // learner instance, initialized according to the input params
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer0_init_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << " brec_learner_init_layer0_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  int ndirs = pro.get_input<int>(i++);
  auto lambda_range = pro.get_input<float>(i++);
  auto lambda_inc = pro.get_input<float>(i++);
  int n = pro.get_input<int>(i++);

  brec_part_hierarchy_learner_sptr hl = new brec_part_hierarchy_learner();
  hl->initialize_layer0_as_gaussians(ndirs, lambda_range, lambda_inc, n);

  pro.set_output_val<brec_part_hierarchy_learner_sptr>(0, hl);

  return true;
}


//: Constructor
bool brec_learner_layer0_fit_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_learner_sptr");     // the already initialized learner instance
  input_types.emplace_back("vcl_string");     // the name of output file to print fitted distributions
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_learner_sptr"); // return learner instance back
  ok = pro.set_output_types(output_types);
  return ok;
}


bool brec_learner_layer0_fit_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_learner_fit_layer0_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);
  std::string output_name = pro.get_input<std::string>(i++);

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
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_learner_sptr");     // the already initialized learner instance
  input_types.emplace_back("int");  // N: top N layer0 prims are used in the created hierarchy
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_sptr");
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer0_rank_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_learner_layer0_rank_process - invalid inputs\n";
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

//: Constructor
bool brec_learner_layer_n_init_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr");     // hierarchy whose layer n will be constructed
  input_types.emplace_back("unsigned");    // n : id of the layer to construct (hierarchy should contain n-1)
  input_types.emplace_back("unsigned");   // k: number of classes
  input_types.emplace_back("float");   // radius that is to be used to collect stats for existence of pairs as they appear within each others neighborhood
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_learner_sptr"); // learner instance, initialized according to the input params
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_learner_layer_n_init_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3){
    std::cerr << " brec_learner_init_layer0_process - invalid inputs\n";
    return false;
  }

  //: get input
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  auto n = pro.get_input<unsigned>(i++);
  auto k = pro.get_input<unsigned>(i++);
  auto rad = pro.get_input<float>(i++);

  brec_part_hierarchy_learner_sptr hl = new brec_part_hierarchy_learner();
  hl->initialize_layer_n_as_pairs(h, n, k, rad);

  pro.set_output_val<brec_part_hierarchy_learner_sptr>(0, hl);

  return true;
}

//: Constructor
bool brec_learner_layer_n_fit_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_learner_sptr");     // the already initialized learner instance
  input_types.emplace_back("unsigned");  // class id
  input_types.emplace_back("unsigned");    // n : id of the layer to construct (hierarchy should contain n-1)
  input_types.emplace_back("vcl_string");     // the name of output file to print fitted distributions
  input_types.emplace_back("unsigned");  // M: number of best pair models to select for this class
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_sptr");  // output the hierarchy for this class, with fitted models for compositions of layer n
  ok = pro.set_output_types(output_types);
  return ok;
}


bool brec_learner_layer_n_fit_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_learner_layer_n_fit_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_learner_sptr hl = pro.get_input<brec_part_hierarchy_learner_sptr>(i++);
  auto class_id = pro.get_input<unsigned>(i++);
  auto n = pro.get_input<unsigned>(i++);
  std::string output_name = pro.get_input<std::string>(i++);
  auto M = pro.get_input<unsigned>(i++);

  hl->layer_n_fit_distributions(class_id, n, M);
  hl->print_to_m_file_layer_n(output_name, class_id, true);

  pro.set_output_val<brec_part_hierarchy_sptr>(0, hl->h_map_[class_id]);

  return true;
}
