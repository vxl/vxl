// This is brl/bseg/boxm/algo/pro/processes/boxm_compute_scene_difference_process.cxx

#include <string>
#include <iostream>
#include <fstream>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process for computing the difference between two scenes.
//
// \author Vishal Jain
// \date Sept 8, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_parser.h>
#include <boxm/algo/boxm_compute_scene_difference.h>

//: global variables
namespace boxm_compute_scene_difference_process_globals
{
  //this process takes no outputs
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

//: sets input and output types
bool boxm_compute_scene_difference_process_cons(bprb_func_process& pro)
{
  using namespace boxm_compute_scene_difference_process_globals;

  // process takes 1 input and no outputs
  //input[0]: The scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "boxm_scene_base_sptr";
  input_types_[2] = "boxm_scene_base_sptr";
  input_types_[3] = "float";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: creates a scene from parameters
bool boxm_compute_scene_difference_process(bprb_func_process& pro)
{
  using namespace boxm_compute_scene_difference_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << "boxm_compute_scene_difference_process: The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene1_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  boxm_scene_base_sptr scene2_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  boxm_scene_base_sptr sceneout_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  auto t = pro.get_input<float>(i++);

  if (scene1_ptr->appearence_model() == BOXM_APM_MOG_GREY &&
      scene2_ptr->appearence_model() == BOXM_APM_MOG_GREY &&
      sceneout_ptr->appearence_model() == BOXM_APM_MOG_GREY )
  {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto *scene1 = static_cast<boxm_scene<tree_type>*> (scene1_ptr.as_pointer());
      auto *scene2 = static_cast<boxm_scene<tree_type>*> (scene2_ptr.as_pointer());
      auto *sceneout = static_cast<boxm_scene<tree_type>*> (sceneout_ptr.as_pointer());

    boxm_compute_scene_difference(*scene1,*scene2,*sceneout,t);
  }
  else {
    std::cout << "boxm_compute_scene_difference_process: undefined APM type" << std::endl;
    return false;
  }

  return true;
}
