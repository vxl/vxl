// This is brl/bseg/brec/pro/processes/brec_hierarchy_processes.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Processes to create/save/load instances of  part hierarchies.
//
// \author Ozge Can Ozcanli
// \date Dec 26, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Feb 03, 2009 - converted process-class to functions which is the new design for bprb processes.
// \endverbatim

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <brec/brec_part_hierarchy_builder.h>
#include <brec/brec_part_hierarchy.h>

//: Constructor
// create hierarchy process may use the builder class or create an empty one to be passed to training processes to learn the hierarchy
bool brec_create_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("bool");      // if true creates an empty hierarchy otherwise returns an instance created by the builder class for the given detector id
  input_types.emplace_back("unsigned");      // detector id for the type of structure to be recognized (if using builder class, otherwise not-used)
  input_types.emplace_back("vcl_string");      // name of the hierarchy
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_sptr");      // output hierarchy
  ok = pro.set_output_types(output_types);
  return ok;
}


bool brec_create_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 3) {
    std::cerr << "brec_create_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  bool create_empty = pro.get_input<bool>(i++);
  auto d_id = pro.get_input<unsigned>(i++);
  std::string name = pro.get_input<std::string>(i++);

  brec_part_hierarchy_sptr h;
  if (create_empty) {
    h = new brec_part_hierarchy();
    h->set_name(name);
  } else {
    switch (d_id) {
      case 0: { h = brec_part_hierarchy_builder::construct_detector_roi1_0(); } break;
      case 1: { h = brec_part_hierarchy_builder::construct_detector_roi1_1(); } break;
      case 2: { h = brec_part_hierarchy_builder::construct_detector_roi1_2(); } break;
      case 3: { h = brec_part_hierarchy_builder::construct_detector_roi1_3(); } break;
      case 4: { h = brec_part_hierarchy_builder::construct_detector_roi1_4(); } break;
      case 100: { h = brec_part_hierarchy_builder::construct_test_detector(); } break;
      default: { std::cout << "In brec_create_hierarchy_process::execute() -- Unrecognized detector type!!\n"; return false; }
    }
  }

  pro.set_output_val<brec_part_hierarchy_sptr>(0, h);

  return true;
}

//: Constructor
bool brec_load_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");      // name of xml file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  output_types.emplace_back("brec_part_hierarchy_sptr");      // h
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_load_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 1) {
    std::cerr << " brec_load_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  std::string name = pro.get_input<std::string>(i++);

  brec_part_hierarchy_sptr h = new brec_part_hierarchy();
  std::ifstream is(name.c_str());
  h->read_xml(is);
  is.close();

  pro.set_output_val<brec_part_hierarchy_sptr>(0, h);

  return true;
}

bool brec_save_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr");      // h
  input_types.emplace_back("vcl_string");      // name of output xml file
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_save_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_load_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  std::string name = pro.get_input<std::string>(i++);

  std::ofstream os(name.c_str());
  h->write_xml(os);
  os.close();

  return true;
}

bool brec_draw_hierarchy_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr");      // h
  input_types.emplace_back("vcl_string");      // name of output ps file
  input_types.emplace_back("float");  // drawing radius for the primitive parts, this radius is doubled as we go up in the hierarchy
  input_types.emplace_back("int");  // number of samples to sample from edge distributions
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_draw_hierarchy_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 4) {
    std::cerr << " brec_load_hierarchy_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  std::string name = pro.get_input<std::string>(i++);
  auto drawing_radius = pro.get_input<float>(i++);
  int N = pro.get_input<int>(i++);

  h->draw_to_ps(N, name, drawing_radius);

  return true;
}

bool brec_set_hierarchy_model_dir_process_cons(bprb_func_process& pro)
{
  //inputs
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("brec_part_hierarchy_sptr");      // h
  input_types.emplace_back("vcl_string");      // model dir
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  //output
  std::vector<std::string> output_types;
  ok = pro.set_output_types(output_types);
  return ok;
}

bool brec_set_hierarchy_model_dir_process(bprb_func_process& pro)
{
  // Sanity check
  if (pro.n_inputs() < 2) {
    std::cerr << " brec_set_hierarchy_model_dir_process - invalid inputs\n";
    return false;
  }

  // get input
  unsigned i = 0;
  brec_part_hierarchy_sptr h = pro.get_input<brec_part_hierarchy_sptr>(i++);
  std::string name = pro.get_input<std::string>(i++);
  h->set_model_dir(name);
  return true;
}
