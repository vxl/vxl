// This is brl/bseg/bstm_multi/pro/processes/bstm_multi_load_scene_process.cxx
//:
// \file
// \brief  A process for loading a bstm_multi_block scene.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <iostream>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bprb/bprb_func_process.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

namespace {
constexpr unsigned n_inputs_ = 1;
constexpr unsigned n_outputs_ = 1;
}

bool bstm_multi_load_scene_process_cons(bprb_func_process &pro) {
  std::vector<std::string> input_types_(::n_inputs_);
  input_types_[0] = "vcl_string"; // scene filename

  // output[0]: scene sptr
  std::vector<std::string> output_types_(::n_outputs_);
  output_types_[0] = "bstm_multi_scene_sptr"; // scene sptr

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_multi_load_scene_process(bprb_func_process &pro) {
  if (pro.n_inputs() < ::n_inputs_) {
    std::cout << pro.name() << ": The input number should be " << ::n_inputs_
             << std::endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  std::string scene_file = pro.get_input<std::string>(i++);
  bstm_multi_scene_sptr scene = new bstm_multi_scene(scene_file);

  i = 0;
  // store scene smaprt pointer
  pro.set_output_val<bstm_multi_scene_sptr>(i++, scene);
  return true;
}
