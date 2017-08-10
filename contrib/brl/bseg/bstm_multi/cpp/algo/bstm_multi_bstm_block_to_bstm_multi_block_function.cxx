#include "bstm_multi_bstm_block_to_bstm_multi_block_function.h"

#include <vcl_cstring.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>

#include <bstm_multi/basic/array_4d.h>
#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

// Imports space trees from a BSTM block into a multi-BSTM block.
//
// This is not a simply memcpy: BSTM's time trees are essentially
// stored as a 4D array (x,y,z,time) in row-major order. This is what
// we want. However, for the space trees, each leaf node points to
// *several* time trees. This way BSTM can store more than 32 frames
// (one time tree's worth) per block. However, we'd like to split it
// up so each space tree leaf node points to one time tree. We'll then
// coalesce as necessary at higher levels.  We would like space trees
// to also be a 4d array in row major order. These means making
// several copies in a row of each space tree.
//
// NOTE: data offsets of the space trees (which point to time blocks)
// are invalidated by this. This doesn't matter for our purposes since
// we'll be re-calculating the offsets once the trees are "unrefined"
// at higher levels of the Multi-BSTM block.
//
// \param blk - Multi-BSTM block into which space trees are copied.
//
// \param bstm_blk - BSTM block containing space trees
//
// \param num_time_trees - the number of time trees per space
// voxel. e.g. the "time width" of the space-time region covered by
// bstm_blk.
//
// \param current_level - The level (0-indexed) of the new Multi-BSTM
// block into which trees are copied. Usually, this level is
// MAX_LEVELS-2, and MAX_LEVELS-1 contains the corresponding time
// trees of the BSTM block.
//
void convert_bstm_space_trees(bstm_multi_block *blk,
                              const bstm_block *bstm_blk,
                              vcl_size_t num_time_trees,
                              vcl_size_t current_level) {
  // Create new space buffer
  vcl_size_t num_space_bytes = bstm_blk->byte_count() * num_time_trees;
  unsigned char *new_space_buffer = new unsigned char[num_space_bytes];
  blk->set_buffer(new_space_buffer, current_level);

  // Wrap new space block in 4D array so we can iterate over trees in row-major
  // order.
  bstm_multi_block::space_tree_b *space_buffer_ptr =
      reinterpret_cast<bstm_multi_block::space_tree_b *>(new_space_buffer);
  vec4 space_block_dims(bstm_blk->sub_block_num().x(),
                        bstm_blk->sub_block_num().y(),
                        bstm_blk->sub_block_num().z(),
                        num_time_trees);
  array_4d<bstm_multi_block::space_tree_b> new_trees(space_buffer_ptr,
                                                     space_block_dims);
  const boxm2_array_3d<bstm_multi_block::space_tree_b> &trees =
      bstm_blk->trees();
  array_4d<bstm_multi_block::space_tree_b>::iterator new_iter =
      new_trees.begin();
  // Create time_trees_per_space_voxel copies of each space tree, in order.
  for (boxm2_array_3d<bstm_multi_block::space_tree_b>::const_iterator iter =
           trees.begin();
       iter != trees.end();
       ++iter) {
    for (int i = 0; i < num_time_trees; ++i, ++new_iter) {
      vcl_memcpy(new_iter, iter, sizeof(bstm_multi_block::space_tree_b));
    }
  }
}

bool get_root_bit(unsigned char *bytes, space_time_enum tree_type) {
  switch (tree_type) {
  case STE_SPACE:
    return boct_bit_tree(bytes).bit_at(0);
  case STE_TIME:
    return bstm_time_tree(bytes).bit_at(0);
  }
}

// Given a space tree and a buffer of lower-level trees, sets the
// structure bits of the given space tree to represent the low-level
// trees. i.e. if a whole bunch of the lower-level trees are
// unrefined, the coresponding inner node for the space tree will also
// be unrefined.
//
// \param current_tree - the space tree that is to contain the
// lower-level trees.
//
// \param num_regions - The number of trees in each dimension at the
// current level (i.e. x, y, z, time).
//
// \param coords - Indices corresponding to current_tree's position in
// the current level.
//
// \param child_level_buffer - Buffer containing trees at the lower
// level. These can be either all space trees or all time trees. Also,
// since a space tree represents a space of 8^=512 sub-voxels, if
// num_regions=[x,y,z,t], then the size of the child level should be
// [x/8, y/8, z/8, t]. Trees should be arranged in row-major order.
//
// \param child_level_type - Whether the lower level contains time
// trees or space trees.
void make_unrefined_space_tree(
    boct_bit_tree &current_tree,
    const vcl_pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const vec4 &coords,
    unsigned char *child_level_buffer,
    space_time_enum child_level_type) { // get current space tree

  // This does not contain data, but useful for converting indices
  // to coordinates.  We can't directly wrap the child buffer
  // because we don't know (at compile time) if it contains time
  // or space trees
  array_4d<int> child_indices(VXL_NULLPTR,
                              num_regions.first.x() * 8,
                              num_regions.first.y() * 8,
                              num_regions.first.z() * 8,
                              num_regions.second);
  // Similar empty array for indices. Note that each leaf
  // node in a space tree contains 8 sub-voxels, so each
  // space tree is actually 4x4x4.
  boxm2_array_3d<int> leaf_node_indices(4, 4, 4, VXL_NULLPTR);
  // iterate over children in lower level (8 in each space
  // dimension)
  for (int i_sub = 0; i_sub < 8; ++i_sub) {
    for (int j_sub = 0; j_sub < 8; ++j_sub) {
      for (int k_sub = 0; k_sub < 8; ++k_sub) {
        // get sub tree (either space or time tree depending on
        // child level type)
        vcl_size_t sub_tree_index =
            child_indices.index_from_coords(coords.i * 8 + i_sub,
                                            coords.j * 8 + j_sub,
                                            coords.k * 8 + k_sub,
                                            coords.t);
        unsigned char *sub_tree_bytes =
            child_level_buffer + (sub_tree_index * tree_size(child_level_type));
        // check root bit. If not zero, we need to keep current
        // space tree refined.
        bool root_bit = get_root_bit(sub_tree_bytes, child_level_type);
        // we want this to be the case some times. Fail so I can notice that
        // it's possible.
        assert(!root_bit);
        // Set corresponding leaf node on our main tree
        if (root_bit) {
          vcl_size_t leaf_index =
              leaf_node_indices.linear_index(i_sub / 2, j_sub / 2, k_sub / 2);
          current_tree.set_bit_and_parents_to_true(leaf_index);
        }
      }
    }
  }
}

void make_unrefined_time_tree(
    bstm_time_tree &current_tree,
    const vcl_pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const vec4 &coords,
    unsigned char *child_level_buffer,
    space_time_enum child_level_type) {
  // stores true if frame is different than the last, false otherwise.
  bool frame_data[32];
  array_4d<int> child_indices(VXL_NULLPTR,
                              num_regions.first.x(),
                              num_regions.first.y(),
                              num_regions.first.z(),
                              num_regions.second * 32);
  unsigned char *last_different_subtree_bytes;
  // leaf index of previous frame that was marked as different (or the first
  // frame)
  vcl_size_t last_different_idx;
  // this time tree covers 32 "frames" from the lower level.
  for (vcl_size_t t_sub = 0; t_sub < 32; ++t_sub) {
    vcl_size_t sub_tree_index = child_indices.index_from_coords(
        coords.i, coords.j, coords.k, coords.t * 32 + t_sub);
    unsigned char *sub_tree_bytes =
        child_level_buffer + (sub_tree_index * tree_size(child_level_type));
    // 15 is start index of leaf nodes, and each leaf node
    // contains two time steps
    int leaf_node_index = 15 + (t_sub / 2);
    if (t_sub == 0) {
      frame_data[t_sub] = true;
      last_different_idx = leaf_node_index;
      last_different_subtree_bytes = sub_tree_bytes;
    } else {
      frame_data[t_sub] =
          compare_frames(sub_tree_bytes, last_different_subtree_bytes);
    }
  }
  current_tree.fill_cells(frame_data);
}

// TODO CURRENT ASSUMPTIONS:
// - bstm block is same size as bstm multi block
// - bstm multi block has space,time subdivisions as its two lowest subdiv
// levels
// - bstm block is already properly refined as per the given thresholds
// - - this means all we're doing right now is copying over BSTM data, and
// un-refining levels above as necessary.

bool bstm_block_to_bstm_multi_block(
    bstm_multi_block *blk,
    const vcl_map<vcl_string, block_data_base *> &datas,
    bstm_block *bstm_blk,
    bstm_time_block *bstm_blk_t,
    const vcl_map<vcl_string, bstm_data_base *> &bstm_datas,
    double p_threshold,
    double app_threshold) {
  // our first two lowest levels are, currently, the same as BSTM block
  // copy time buffer into multi-bstm block
  int current_level = blk->metadata().subdivisions_.size() - 1;
  vcl_size_t num_time_bytes = bstm_blk_t->byte_count();
  unsigned char *new_time_buffer = new unsigned char[num_time_bytes];
  blk->set_buffer(new_time_buffer, current_level);
  vcl_memcpy(new_time_buffer, bstm_blk_t->buffer(), num_time_bytes);
  --current_level;

  convert_bstm_space_trees(
      blk, bstm_blk, bstm_blk_t->sub_block_num(), current_level);
  --current_level;

  // Keeps track of the number of space/time regions at each level
  // This starts out equal to the number of blocks in the BSTM block, but one
  // level up.
  vcl_pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
      bstm_blk->sub_block_num(), bstm_blk_t->sub_block_num());
  // moving upwards through levels:
  for (; current_level >= 0; --current_level) {
    space_time_enum current_level_type =
        blk->metadata().subdivisions_[current_level];
    space_time_enum child_level_type =
        blk->metadata().subdivisions_[current_level + 1];
    unsigned char *child_level_buffer = blk->buffers()[current_level + 1];
    // find proper refinement for this level
    switch (current_level_type) {
    case STE_SPACE: {
      // Create new empty buffer for this level
      num_regions.first /= 8;
      vcl_size_t num_trees = num_regions.first.x() * num_regions.first.y() *
                             num_regions.first.z() * num_regions.second;
      bstm_multi_block::space_tree_b *current_buffer =
          new bstm_multi_block::space_tree_b[num_trees];
      blk->set_buffer(reinterpret_cast<unsigned char *>(current_buffer),
                      current_level);
      vcl_memset(current_buffer,
                 0,
                 num_trees * sizeof(bstm_multi_block::space_tree_b));
      array_4d<bstm_multi_block::space_tree_b> trees(current_buffer,
                                                     num_regions.first.x(),
                                                     num_regions.first.y(),
                                                     num_regions.first.z(),
                                                     num_regions.second);
      for (int i = 0; i < trees.size(); ++i) {
        boct_bit_tree current_tree(trees[i]);
        make_unrefined_space_tree(current_tree,
                                  num_regions,
                                  trees.coords_from_index(i),
                                  child_level_buffer,
                                  child_level_type);
      }
      break;
    }
    case STE_TIME: {
      num_regions.second /= 32;
      vcl_size_t num_trees = num_regions.first.x() * num_regions.first.y() *
                             num_regions.first.z() * num_regions.second;
      bstm_multi_block::time_tree_b *current_buffer =
          new bstm_multi_block::time_tree_b[num_trees];
      blk->set_buffer(reinterpret_cast<unsigned char *>(current_buffer),
                      current_level);
      vcl_memset(
          current_buffer, 0, num_trees * sizeof(bstm_multi_block::time_tree_b));
      array_4d<bstm_multi_block::time_tree_b> trees(current_buffer,
                                                    num_regions.first.x(),
                                                    num_regions.first.y(),
                                                    num_regions.first.z(),
                                                    num_regions.second);
      for (int i = 0; i < trees.size(); ++i) {
        bstm_time_tree current_tree(trees[i]);
        make_unrefined_time_tree(current_tree,
                                 num_regions,
                                 trees.coords_from_index(i),
                                 child_level_buffer,
                                 child_level_type);
      }
      // - - - go over each time region and check if it is "the same" as the
      //       previous (technically, the same as the last "differnt" one)
      // - - - definition of "same":
      // - - - - every space region is either unrefined or the same
      // - - - - - the same means data is similar, within threshold
      // - - - - - if time sub-region in lower level of either time
      //           region is to more than one node, then they are different. If
      //           both
      //           have one node, each node must be the same
      // - - - NOTE: we can do this by getting "similarity" information from
      // BSTM
      //       time trees initially.
      // - - - - we can also coalesce this at each level if that helps
      //         from the top down:
      break;
    }
    }
  }

  // use leaf counts to create copy of lower level that has only the trees it
  //   needs
  // use leaf counts to set offsets into next lower level
  //   for data buffers, create copy creating only data for refined nodes
}
