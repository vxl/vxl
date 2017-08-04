// This is brl/bseg/bstm/pro/processes/bstm_scene_lvcs_process.cxx
//:
// \file
// \brief  A process for accessing scene LVCS
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vpgl/vpgl_lvcs_sptr.h>

#include <bprb/bprb_func_process.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

namespace {
const unsigned n_inputs_ = 1;
const unsigned n_outputs_ = 1;
}

bool bstm_scene_lvcs_process_cons(bprb_func_process &pro) {
  vcl_vector<vcl_string> input_types_(::n_inputs_);
  input_types_[0] = "bstm_scene_sptr";

  vcl_vector<vcl_string> output_types_(::n_outputs_);
  output_types_[0] = "vpgl_lvcs_sptr";
  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_scene_lvcs_process(bprb_func_process &pro) {
  if (pro.n_inputs() < ::n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << ::n_inputs_
             << vcl_endl;
    return false;
  }

  // get the inputs
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(0);
  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());

  // store outputs
  int i = 0;
  pro.set_output_val<vpgl_lvcs_sptr>(i++, lvcs);
  return true;
}
