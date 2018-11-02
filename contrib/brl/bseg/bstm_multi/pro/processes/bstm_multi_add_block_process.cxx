// This is brl/bseg/bstm_multi/pro/processes/bstm_multi_add_block_process.cxx

//:
// \file bstm_multi_add_block_process.cxx
// \brief  A process for adding a block to a scene. Caller specifies all
// metadata, including subdivisions. Block is only added if there is no existing
// block with the same ID.
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_box_3d.h>

#include <bprb/bprb_func_process.h>
#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

namespace {
constexpr unsigned n_inputs_ = 21;
constexpr unsigned n_outputs_ = 0;
}

bool bstm_add_block_process_cons(bprb_func_process &pro) {
  std::vector<std::string> input_types_(::n_inputs_);
  int i = 0;
  input_types_[i++] = "bstm_multi_scene_sptr";

  input_types_[i++] = "int"; // index i
  input_types_[i++] = "int"; // index j
  input_types_[i++] = "int"; // index k
  input_types_[i++] = "int"; // index t

  input_types_[i++] = "double"; // min i
  input_types_[i++] = "double"; // min j
  input_types_[i++] = "double"; // min k
  input_types_[i++] = "double"; // min t

  input_types_[i++] = "double"; // max i
  input_types_[i++] = "double"; // max j
  input_types_[i++] = "double"; // max k
  input_types_[i++] = "double"; // max t

  input_types_[i++] = "vcl_string"; // subdivisions as comma-separated set of
                                    // 'space' or 'time' values.
  input_types_[i++] = "float";      // max_mb
  input_types_[i++] = "float";      // p_init
  input_types_[i++] = "unsigned";   // init tree depth
  input_types_[i++] = "unsigned";   // init time tree depth

  std::vector<std::string> output_types_(::n_outputs_);

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types_);
}

bool bstm_add_block_process(bprb_func_process &pro) {
  if (pro.n_inputs() < ::n_inputs_) {
    std::cout << pro.name() << ": The input number should be " << ::n_inputs_
             << std::endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(i++);
  int index_i = pro.get_input<int>(i++);
  int index_j = pro.get_input<int>(i++);
  int index_k = pro.get_input<int>(i++);
  int index_t = pro.get_input<int>(i++);

  // Scene bounds
  auto min_x = pro.get_input<double>(i++);
  auto min_y = pro.get_input<double>(i++);
  auto min_z = pro.get_input<double>(i++);
  auto min_t = pro.get_input<double>(i++);

  auto max_x = pro.get_input<double>(i++);
  auto max_y = pro.get_input<double>(i++);
  auto max_z = pro.get_input<double>(i++);
  auto max_t = pro.get_input<double>(i++);

  std::string subdivisions_str = pro.get_input<std::string>(i++);
  std::vector<space_time_enum> subdivisions =
      parse_subdivisions(subdivisions_str);

  auto max_data_size = pro.get_input<float>(i++);
  auto p_init = pro.get_input<float>(i++);

  bstm_block_id id(index_i, index_j, index_k, index_t);
  std::map<bstm_block_id, bstm_multi_block_metadata> &blks = scene->blocks();
  if (blks.count(id)) {
    std::cout << "block already exists" << std::endl;
    return false;
  }
  blks[id] = bstm_multi_block_metadata(
      id,
      vgl_box_3d<double>(min_x, min_y, min_z, max_x, max_y, max_z),
      std::pair<double, double>(min_t, max_t),
      max_data_size,
      p_init,
      subdivisions,
      2);

  return true;
}
