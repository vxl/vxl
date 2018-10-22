// This is brl/bseg/boxm2/pro/processes/boxm2_load_scene_process.cxx
//:
// \file
// \brief  A process for loading the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>

#include <boxm2/boxm2_scene.h>

namespace boxm2_load_scene_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}
bool boxm2_load_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_load_scene_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "boxm2_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_load_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  std::string scene_file = pro.get_input<std::string>(i++);
  boxm2_scene_sptr scene;
  try {
    scene = new boxm2_scene(scene_file);
  }
  catch(const std::ifstream::failure &e) {
    return false;
  }

  i=0;
  // store scene smaprt pointer
  pro.set_output_val<boxm2_scene_sptr>(i++, scene);
  return true;
}

namespace boxm2_modify_scene_appearance_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 0;
}
bool boxm2_modify_scene_appearance_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_modify_scene_appearance_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_modify_scene_appearance_process(bprb_func_process& pro)
{
  using namespace boxm2_load_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr s = pro.get_input<boxm2_scene_sptr>(i++);
  std::string apptype1 = pro.get_input<std::string>(i++);
  std::string apptype2 = pro.get_input<std::string>(i++);

  std::vector<std::string> apps;

  apps.push_back(apptype1);
  apps.push_back(apptype2);
  s->set_appearances(apps);

  return true;
}
