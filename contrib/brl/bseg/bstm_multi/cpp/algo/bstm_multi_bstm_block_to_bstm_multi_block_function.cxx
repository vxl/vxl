#include "bstm_multi_bstm_block_to_bstm_multi_block_function.h"

#include <vcl_cstring.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_data.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>
#include <bstm/cpp/algo/bstm_data_similarity_traits.h>

#include <bstm_multi/basic/array_4d.h>
#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

// \brief Imports space and time trees from a BSTM block into a multi-BSTM
// block.
//
// This is not a simple memcpy. BSTM's space trees are stored in row-major of
// (x,y,z) coordinates. Each space tree points to a buffer of time trees
// containing num_time_trees * num_leaves such trees. Specifically, each *leaf*
// of the space tree points to num_time_trees contiguous time trees.
//
// We would like to have (x,y,z,num_time_trees) space trees in row-major order,
// where each space tree points to num_leaves contiguous time trees. Thus we
// need to shuffle the time trees (and data buffers) around, as well as create
// num_time_trees extra copies of each space tree.
//
// \param blk - Multi-BSTM block into which space trees are copied.
//
// \param bstm_blk - BSTM block containing space trees
//
// \param bstm_blk_t - BSTM time block containing time trees
//
// \param current_level - The level (0-indexed) of the new Multi-BSTM
// block into which trees are copied. Usually, this level is
// MAX_LEVELS-1. (i.e. time and space trees are copied into the last two levels)
//
// \param alpha - BSTM's occupancy data buffer
//
// \param alpha_new - Multi-BSTM's occupancy data buffer, which is replaced with
// a new data buffer the same size as 'alpha' containing copied-over occupancy
// data.
//
// \param appearance - BSTM's appearance data buffer
//
// \param appearance_new - Multi-BSTM's appearance data buffer, which
// is replaced with a new data buffer the same size as 'appearance'
// containing copied-over appearance data.
//
// \param appearance_type - apearance data type
void convert_bstm_space_trees(bstm_multi_block *blk,
                              bstm_block *bstm_blk,
                              bstm_time_block *bstm_blk_t,
                              vcl_size_t current_level,
                              bstm_data_base *alpha,
                              block_data_base *alpha_new,
                              bstm_data_base *appearance,
                              block_data_base *appearance_new,
                              const vcl_string &appearance_type) {
  vcl_size_t app_type_size = bstm_data_info::datasize(appearance_type);
  vcl_size_t alpha_type_size = bstm_data_traits<BSTM_ALPHA>::datasize();
  vcl_size_t num_time_trees = bstm_blk_t->sub_block_num();
  vcl_size_t num_time_bytes = bstm_blk_t->byte_count();
  // Create new time buffer
  unsigned char *new_time_buffer = new unsigned char[num_time_bytes];
  blk->set_buffer(new_time_buffer, current_level);
  // Create new space buffer
  vcl_size_t num_space_bytes = bstm_blk->byte_count() * num_time_trees;
  unsigned char *new_space_buffer = new unsigned char[num_space_bytes];
  blk->set_buffer(new_space_buffer, current_level - 1);

  // Create copies of BSTM data buffers
  alpha_new->new_data_buffer(alpha->buffer_length());
  appearance_new->new_data_buffer(appearance->buffer_length());

  // Wrap new space block in 4D array so we can iterate over trees in row-major
  // order.
  bstm_multi_block::space_tree_b *space_buffer_ptr =
      reinterpret_cast<bstm_multi_block::space_tree_b *>(new_space_buffer);
  vec4 space_block_dims(bstm_blk->sub_block_num().x(),
                        bstm_blk->sub_block_num().y(),
                        bstm_blk->sub_block_num().z(),
                        num_time_trees);
  boxm2_array_3d<bstm_multi_block::space_tree_b> &trees = bstm_blk->trees();
  boxm2_array_3d<bstm_multi_block::space_tree_b>::iterator tree_iter =
      trees.begin();
  array_4d<bstm_multi_block::space_tree_b> new_trees(space_buffer_ptr,
                                                     space_block_dims);
  array_4d<bstm_multi_block::space_tree_b>::iterator new_tree_iter =
      new_trees.begin();
  bstm_multi_block::time_tree_b *time_buffer_ptr =
      reinterpret_cast<bstm_multi_block::time_tree_b *>(new_time_buffer);

  // Offset pointing to start of time trees (in new time buffer) for current
  // space tree.
  vcl_size_t time_tree_offset = 0;
  // Essentially the number of elements already inserted into new data buffer.
  // Actually array index (.e.g to pass to memcpy) is data_buffer_offset *
  // sizeof(datatype)
  vcl_size_t data_buffer_offset = 0;
  // Create time_trees_per_space_voxel copies of each space tree, in order.
  for (; tree_iter != trees.end(); ++tree_iter) {
    boct_bit_tree current_tree(*tree_iter);
    int num_space_cells = current_tree.num_cells();
    vcl_vector<int> cells = current_tree.get_cell_bits();

    // copy over time trees
    for (vcl_size_t cell_idx = 0; cell_idx <= cells.size(); ++cell_idx) {
      boxm2_array_1d<bstm_multi_block::time_tree_b> cell_time_trees =
          bstm_blk_t->get_cell_all_tt(
              current_tree.get_data_index(cells[cell_idx]));
      // copy time trees
      for (vcl_size_t i = 0; i < num_time_trees; ++i) {
        // offset from this space tree's data_ptr that points to current cell at
        // current time step.
        // Note that we want "time major" order - all cells at each time step
        // are stored contiguously.
        vcl_size_t current_tt_index = num_space_cells * i + cell_idx;
        time_buffer_ptr[time_tree_offset + current_tt_index] =
            cell_time_trees[i];
      }
    }

    // Offset relative to start of data buffer for current space tree
    vcl_size_t space_tree_data_offset = 0;
    // Copy over data buffers in proper order and update time tree data pointers
    for (int tt_offset = time_tree_offset;
         tt_offset < num_space_cells * num_time_trees;
         ++tt_offset) {
      bstm_time_tree tt(time_buffer_ptr[tt_offset]);
      int num_leaves = tt.num_leaves();
      int data_ptr = tt.get_data_ptr();
      int new_data_ptr = data_buffer_offset + space_tree_data_offset;
      // copy alpha and appearance from data_ptr to new_data_ptr
      vcl_memcpy(alpha_new->data_buffer() + alpha_type_size * new_data_ptr,
                 alpha->data_buffer() + alpha_type_size * data_ptr,
                 num_leaves * alpha_type_size);
      vcl_memcpy(appearance_new->data_buffer() + app_type_size * new_data_ptr,
                 appearance->data_buffer() + app_type_size * data_ptr,
                 num_leaves * app_type_size);
      // update data pointer for new time trees
      tt.set_data_ptr(new_data_ptr);
      space_tree_data_offset += num_leaves;
    }
    data_buffer_offset += space_tree_data_offset;

    // copy over space trees and update data pointer
    for (vcl_size_t i = 0; i < num_time_trees; ++i, ++new_tree_iter) {
      *new_tree_iter = *tree_iter;
      boct_bit_tree(*new_tree_iter)
          .set_data_ptr(time_tree_offset + num_space_cells * i);
    }
    time_tree_offset += num_space_cells * num_time_trees;
  }
}

// \brief Given BSTM data, creates a 4D array of booleans that
// determines, for each region of space (corresponding to a space tree
// in BSTM), whether that region is the same as the corresponding
// region of space in the previous time frame.  This array is used to
// further coalesce the BSTM scene over time during the creation of
// the multi-BSTM scene. Two time frames are similar if they are a)
// totally unrefined or b) composed of one voxel who's appearance and
// occupancy data changes less than the given threshold over those two
// time frames.
//
// \tparam APP_TYPE - appearance data type.
//
// \param time_buffer - BSTM time trees, arranged in row major order over
// (x,y,z,t)
// \param space_buffer - BSTM space trees, arranged in row major order over
// (x,y,z,t). Note that each voxel of a space tree points to exactly one time
// tree. That is, instead of directly using BSTM space trees, use output from
// convert_bstm_space_trees. Essentially, if the scene is too long over time to
// store in one time tree, there should be one space tree for each 32-frame time
// region.
// \param num_regions - Number of space trees in each x,y,z,t dimension.
// \param alpha - Buffer of occupancy data
// \param app_threshold - Buffer of appearance data
template <bstm_data_type APP_TYPE>
vcl_vector<bool> time_differences_from_bstm(
    unsigned char *time_buffer,
    unsigned char *space_buffer,
    const vcl_pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const block_data<BSTM_ALPHA> &alpha,
    const block_data<APP_TYPE> &appearance,
    double p_threshold,
    double app_threshold) {
  typedef typename bstm_data_traits<BSTM_ALPHA>::datatype alpha_data_type;
  typedef typename bstm_data_traits<APP_TYPE>::datatype app_data_type;
  // used for converting between 4D coordinates and array indices
  array_4d<bstm_multi_block::space_tree_b> space_trees_array(
      reinterpret_cast<bstm_multi_block::space_tree_b *>(space_buffer),
      num_regions.first.x(),
      num_regions.first.y(),
      num_regions.first.z(),
      num_regions.second);
  vcl_vector<bool> time_differences(space_trees_array.size(), false);
  for (vcl_size_t i = 0; i < space_trees_array.x(); ++i) {
    for (vcl_size_t j = 0; j < space_trees_array.y(); ++j) {
      for (vcl_size_t k = 0; k < space_trees_array.z(); ++k) {
        // arrays are stored in row-major order, so we can directly iterate over
        // time dimension.
        vcl_size_t first_frame_idx =
            space_trees_array.index_from_coords(i, j, k, 0);
        boct_bit_tree last_same_tree(space_trees_array[first_frame_idx]);
        for (vcl_size_t idx = first_frame_idx; idx < first_frame_idx + 32;
             ++idx) {
          boct_bit_tree current_tree(space_trees_array[idx]);
          // If trees have different structure, then naturally they are
          // different.
          if (!boct_bit_tree::same_structure(last_same_tree, current_tree)) {
            time_differences[idx] = true;
            last_same_tree = current_tree;
            continue;
          }

          // Same as leaf bit indices in last_same_tree
          vcl_vector<int> leaf_bits = current_tree.get_leaf_bits();
          for (vcl_vector<int>::const_iterator iter = leaf_bits.begin();
               iter != leaf_bits.end();
               ++iter) {
            vcl_size_t current_tt_idx = current_tree.get_data_index(*iter);
            vcl_size_t previous_tt_idx = last_same_tree.get_data_index(*iter);
            bstm_time_tree current_tt(time_buffer + current_tt_idx);
            bstm_time_tree previous_tt(time_buffer + previous_tt_idx);
            // If time trees are refined at all, then they have more than one
            // frame and are not a single identical region. Do not refine.
            if (current_tt.bit_at(0) || previous_tt.bit_at(0)) {
              time_differences[idx] = true;
              last_same_tree = current_tree;
              break;
            }
            // Now compare actual data values
            vcl_size_t current_data_idx = current_tt.get_data_index(0);
            vcl_size_t previous_data_idx = previous_tt.get_data_index(0);
            const alpha_data_type &current_alpha = alpha[current_data_idx];
            const alpha_data_type &previous_alpha = alpha[previous_data_idx];
            const app_data_type &current_app = appearance[current_data_idx];
            const app_data_type &previous_app = appearance[previous_data_idx];
            if (!is_similar<APP_TYPE>(previous_app,
                                      current_app,
                                      previous_alpha,
                                      current_alpha,
                                      p_threshold,
                                      app_threshold)) {
              time_differences[idx] = true;
              last_same_tree = current_tree;
              break;
            }
          }
        }
      }
    }
  }
  return time_differences;
}

#define TIME_DIFFERENCES_INSTANTIATE(APP_TYPE)                                 \
  return time_differences_from_bstm<APP_TYPE>(                                 \
      time_buffer,                                                             \
      space_buffer,                                                            \
      num_regions,                                                             \
      block_data<BSTM_ALPHA>::const_wrapper(*alpha),                           \
      block_data<APP_TYPE>::const_wrapper(*appearance),                        \
      p_threshold,                                                             \
      app_threshold);

#define TIME_DIFFERENCES_CASE(APP_TYPE)                                        \
  case APP_TYPE:                                                               \
    TIME_DIFFERENCES_INSTANTIATE(APP_TYPE);

vcl_vector<bool> dispatch_time_differences_from_bstm(
    unsigned char *time_buffer,
    unsigned char *space_buffer,
    const vcl_pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const block_data_base *alpha,
    const block_data_base *appearance,
    const vcl_string &appearance_type,
    double p_threshold,
    double app_threshold) {
  switch (bstm_data_info::data_type(appearance_type)) {
    TIME_DIFFERENCES_CASE(BSTM_MOG3_GREY);
    TIME_DIFFERENCES_CASE(BSTM_MOG6_VIEW);
    TIME_DIFFERENCES_CASE(BSTM_MOG6_VIEW_COMPACT);
    TIME_DIFFERENCES_CASE(BSTM_GAUSS_RGB);
    TIME_DIFFERENCES_CASE(BSTM_GAUSS_RGB_VIEW_COMPACT);
  default:
    return vcl_vector<bool>();
  }
}

#undef TIME_DIFFERENCES_INSTANTIATE
#undef TIME_DIFFERENCES_CASE

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
    space_time_enum child_level_type,
    const vcl_vector<bool> &time_differences_vec) { // get current space tree

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
    space_time_enum child_level_type,
    const vcl_vector<bool> &time_differences_vec) {
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
      // // XXX TODO
      // frame_data[t_sub] =
      //     compare_frames(sub_tree_bytes, last_different_subtree_bytes);
    }
  }
  current_tree.fill_cells(frame_data);
}

// Gets an appearance and an alpha model from a collection of data buffers. If
// there are multiple appearance models, only the first one is used. The buffers
// are stored in 'alpha' and 'appearance', respectively.
void get_bstm_data_buffers(
    const vcl_map<vcl_string, bstm_data_base *> &bstm_datas,
    bstm_data_base *&alpha,
    bstm_data_base *&appearance,
    vcl_string &appearance_type) {
  vcl_vector<vcl_string> allowed_appearance_models;
  allowed_appearance_models.push_back(
      bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  allowed_appearance_models.push_back(
      bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  allowed_appearance_models.push_back(
      bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  allowed_appearance_models.push_back(
      bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  allowed_appearance_models.push_back(
      bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());

  vcl_map<vcl_string, bstm_data_base *>::const_iterator iter =
      bstm_datas.find(bstm_data_traits<BSTM_ALPHA>::prefix());
  if (iter != bstm_datas.end()) {
    alpha = iter->second;
  } else {
    return;
  }

  for (vcl_vector<vcl_string>::const_iterator app_iter =
           allowed_appearance_models.begin();
       app_iter != allowed_appearance_models.end();
       ++app_iter) {
    iter = bstm_datas.find(*app_iter);
    if (iter != bstm_datas.end()) {
      appearance = iter->second;
      appearance_type = iter->first;
    }
  }
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
    vcl_map<vcl_string, block_data_base *> &datas,
    bstm_block *bstm_blk,
    bstm_time_block *bstm_blk_t,
    const vcl_map<vcl_string, bstm_data_base *> &bstm_datas,
    double p_threshold,
    double app_threshold) {
  // extract BSTM data buffers
  bstm_data_base *alpha = VXL_NULLPTR, *appearance = VXL_NULLPTR;
  vcl_string appearance_type;
  get_bstm_data_buffers(bstm_datas, alpha, appearance, appearance_type);
  if (alpha == VXL_NULLPTR || appearance == VXL_NULLPTR) {
    vcl_cerr << "Could not find either alpha or appearance model in bstm_datas."
             << vcl_endl;
    return false;
  }

  // our first two lowest levels are, currently, the same as BSTM block
  // copy time buffer into multi-bstm block
  int current_level = blk->metadata().subdivisions_.size() - 1;
  convert_bstm_space_trees(blk,
                           bstm_blk,
                           bstm_blk_t,
                           current_level,
                           alpha,
                           datas["BSTM_ALPHA"],
                           appearance,
                           datas[appearance_type],
                           appearance_type);
  current_level -= 2;

  // Keeps track of the number of space/time regions at each level
  // This starts out equal to the number of blocks in the BSTM block, but one
  // level up.
  vcl_pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
      bstm_blk->sub_block_num(), bstm_blk_t->sub_block_num());
  vcl_vector<bool> time_differences_vec =
      dispatch_time_differences_from_bstm(blk->buffers()[current_level + 2],
                                          blk->buffers()[current_level + 1],
                                          num_regions,
                                          datas["BSTM_ALPHA"],
                                          datas[appearance_type],
                                          appearance_type,
                                          p_threshold,
                                          app_threshold);

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
      vcl_memset(
          current_buffer, 0, num_trees * bstm_multi_block::space_tree_size);
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
                                  child_level_type,
                                  time_differences_vec);
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
          current_buffer, 0, num_trees * bstm_multi_block::time_tree_size);
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
                                 child_level_type,
                                 time_differences_vec);
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
