//This is brl/bseg/boxm/pro/processes/boxm_force_write_blocks_process.cxx
#include <string>
#include <iostream>
#include <fstream>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process for removing any previously created scene data from disk
//
// \author Daniel Crispell
// \date Feb 18, 2010
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

#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/boxm_scene_parser.h>

//:global variables
namespace boxm_force_write_blocks_process_globals
{
  //this process takes no inputs
  constexpr unsigned int n_inputs_ = 1;
  constexpr unsigned int n_outputs_ = 0;
}

//:sets input and output types
bool boxm_force_write_blocks_process_cons(bprb_func_process& pro)
{
  //set output types
  using namespace boxm_force_write_blocks_process_globals;
  //process takes 1 input
  //input[0]: The scene
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  return true;
}

//: removes data previously associated with scene
bool boxm_force_write_blocks_process(bprb_func_process& pro)
{
  using namespace boxm_force_write_blocks_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << "boxm_force_write_blocks_process: The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);

  // check the input validity
  if (scene == nullptr) {
    std::cout << "boxm_force_write_blocks_process: scene is null, cannot run" << std::endl;
    return false;
  }

  switch (scene->appearence_model())
  {
   case BOXM_APM_MOG_GREY:
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      s->force_write_blocks();
      break;
    }
   case BOXM_APM_SIMPLE_GREY:
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      s->force_write_blocks();
      break;
    }
   case BOXM_APM_MOB_GREY:
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      s->force_write_blocks();
      break;
    }
   default:
    std::cout << "boxm_force_write_blocks_process: undefined APM type" << std::endl;
    return false;
  }
  return true;
}
