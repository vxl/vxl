// This is brl/bseg/boxm/algo/pro/processes/boxm_refine_scene_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to refine the block (octrees) of the scene
//        If there are some cells with big values they are spit into
//        new child cells; the data of the cell is copied to the
//        children's data. The purpose of this process is to elaborate
//        the octree at the areas where more details exist.
//
// \author Gamze Tunali
// \date April 24, 2009

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_refine.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/sample/boxm_sample_multi_bin.h>

namespace boxm_refine_scene_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}

bool boxm_refine_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_refine_scene_process_globals;

  //process takes 3 inputs
  //input[0]: The scene
  //input[1]: The threshold for splitting
  //input[2]: bool for resetting the appearence model
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float";
  input_types_[2] = "bool";

  //process has 1 output
  //output[0]: The number of leaf cells in the refined scene
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "int";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_refine_scene_process(bprb_func_process& pro)
{
  using namespace boxm_refine_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << "boxm_refine_scene_process: The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  auto thresh = pro.get_input<float>(i++);
  bool reset = pro.get_input<bool>(i++);

  // check the input validity
  if (scene == nullptr) {
    std::cout << "boxm_refine_scene_process: scene is null, cannot run" << std::endl;
    return false;
  }

  unsigned int ncells = 0;

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    if (scene->multi_bin()) {
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      ncells = boxm_refine_scene<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*s, thresh, reset);
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      ncells = boxm_refine_scene<short, boxm_sample<BOXM_APM_MOG_GREY> >(*s, thresh, reset);
    }
  }
  else if (scene->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    if (scene->multi_bin()) {
      std::cout << "boxm_refine_scene_process: multibin case is not implemented for BOXM_APM_SIMPLE_GREY yet" << std::endl;
      return false;
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      ncells = boxm_refine_scene<short, boxm_sample<BOXM_APM_SIMPLE_GREY> >(*s, thresh, reset);
    }
  }
  else if (scene->appearence_model() == BOXM_APM_MOB_GREY) {
    if (scene->multi_bin()) {
      std::cout << "boxm_refine_scene_process: multibin case is not implemented for BOXM_APM_SIMPLE_GREY yet" << std::endl;
      return false;
    }
    else {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      ncells = boxm_refine_scene<short, boxm_sample<BOXM_APM_MOB_GREY> >(*s, thresh, reset);
    }
  }
  else {
    std::cout << "boxm_refine_scene_process: undefined APM type" << std::endl;
    return false;
  }

  // set output
  pro.set_output_val<int>(0,ncells);

  return true;
}
