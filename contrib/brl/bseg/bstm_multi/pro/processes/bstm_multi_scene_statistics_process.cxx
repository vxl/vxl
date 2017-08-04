// This is
// brl/bseg/bstm_multi/pro/processes/bstm_multi_scene_statistics_process.cxx
//:
// \file
// \brief  A process for printing out various statistics of a given multi-BSTM
// scene.
//
// TODO currently incomplete
//
// \author Raphael Kargon
// \date 04 Aug 2017

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vgl/vgl_intersection.h>

#include <boct/boct_bit_tree.h>
#include <boxm2/basic/boxm2_array_3d.h>
#include <bprb/bprb_func_process.h>
#include <bstm_multi/bstm_multi_block.h>
#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/io/block_cache.h>
#include <bstm_multi/space_time_scene.h>

namespace {
const unsigned n_inputs_ = 8;
const unsigned n_outputs_ = 3;
}

bool bstm_multi_scene_statistics_process_cons(bprb_func_process &pro) {
  vcl_vector<vcl_string> input_types_(::n_inputs_);
  input_types_[0] = "bstm_multi_scene_sptr";
  input_types_[1] = "bstm_multi_cache_sptr";
  input_types_[2] = "float"; // center x
  input_types_[3] = "float"; // center y
  input_types_[4] = "float"; // center z
  input_types_[5] = "float"; // len x
  input_types_[6] = "float"; // len y
  input_types_[7] = "float"; // len z

  vcl_vector<vcl_string> output_types(::n_outputs_);
  output_types[0] = "float";
  output_types[1] = "float";
  output_types[2] = "unsigned";

  brdb_value_sptr def_center_x = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_center_y = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_center_z = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_len_x = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_len_y = new brdb_value_t<float>(-1.0f);
  brdb_value_sptr def_len_z = new brdb_value_t<float>(-1.0f);
  pro.set_input(2, def_center_x);
  pro.set_input(3, def_center_y);
  pro.set_input(4, def_center_z);
  pro.set_input(5, def_len_x);
  pro.set_input(6, def_len_y);
  pro.set_input(7, def_len_z);

  return pro.set_input_types(input_types_) &&
         pro.set_output_types(output_types);
}

bool bstm_multi_scene_statistics_process(bprb_func_process &pro) {
  typedef unsigned char uchar;
  typedef unsigned short ushort;
  typedef vnl_vector_fixed<uchar, 16> uchar16;
  typedef vnl_vector_fixed<uchar, 8> uchar8;
  typedef vnl_vector_fixed<ushort, 4> ushort4;

  if (pro.n_inputs() < ::n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << ::n_inputs_
             << vcl_endl;
    return false;
  }
  // get the inputs
  unsigned i = 0;
  bstm_multi_scene_sptr scene = pro.get_input<bstm_multi_scene_sptr>(i++);
  bstm_multi_cache_sptr cache = pro.get_input<bstm_multi_cache_sptr>(i++);
  float center_x = pro.get_input<float>(i++);
  float center_y = pro.get_input<float>(i++);
  float center_z = pro.get_input<float>(i++);
  float len_x = pro.get_input<float>(i++);
  float len_y = pro.get_input<float>(i++);
  float len_z = pro.get_input<float>(i++);

  // create vgl box
  const vgl_point_3d<double> center(center_x, center_y, center_z);
  vgl_box_3d<double> box(
      center, len_x, len_y, len_z, vgl_box_3d<double>::centre);
  if (len_x <= 0.0f || len_y <= 0.0f ||
      len_z <= 0.0f) // if box is empty, set it to scene's
    box = scene->bounding_box();

  unsigned total_num_time_tree_leaf_cells = 0;
  unsigned num_time_trees = 0;
  unsigned total_time_tree_depths = 0;
  unsigned num_cells = 0;

  // get blocks
  const vcl_map<bstm_block_id, bstm_multi_block_metadata> &blocks =
      scene->blocks();
  vcl_map<bstm_block_id, bstm_multi_block_metadata>::const_iterator
      bstm_multi_iter = blocks.begin();
  for (; bstm_multi_iter != blocks.end(); ++bstm_multi_iter) {
    bstm_block_id block_id = bstm_multi_iter->first;
    bstm_multi_block_metadata bstm_multi_block_metadata =
        bstm_multi_iter->second;

    if (!vgl_intersection<double>(bstm_multi_block_metadata.bbox(), box)
             .is_empty()) // if the two boxes intersect
    {
      bstm_multi_block *blk = cache->get_block(scene, block_id);
      // TODO....
    }
  }

  pro.set_output_val<float>(0, total_num_time_tree_leaf_cells);
  pro.set_output_val<float>(1, float(total_time_tree_depths) / num_time_trees);
  pro.set_output_val<unsigned>(2, num_cells);
  return true;
}
