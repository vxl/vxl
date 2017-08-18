#include "bstm_multi_bstm_block_to_bstm_multi_block_function.h"

#include <vcl_cstring.h>
#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_utility.h>

#include <vnl_vector_fixed.h>

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_data.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>
#include <bstm/cpp/algo/bstm_data_similarity_traits.h>

#include <bstm_multi/basic/array_4d.h>
#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_tree_util.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

template <typename T> T volume(const vcl_pair<vgl_vector_3d<T>, T> &v) {
  return v.first.x() * v.first.y() * v.first.z() * v.second;
}

void get_bstm_data_buffers(
    const vcl_map<vcl_string, bstm_data_base *> &bstm_datas,
    bstm_data_base *&alpha,
    bstm_data_base *&appearance,
    vcl_string &appearance_type) {
  // Only these data types supported. Primarily this has to do with whether or
  // not `is_similar` is implemented, to determine how similar two data elements
  // are. See 'bstm/cpp/algo/bstm_data_similarity_traits.h'.
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

  // get alpha, which is always BSTM_ALPHA
  vcl_map<vcl_string, bstm_data_base *>::const_iterator iter =
      bstm_datas.find(bstm_data_traits<BSTM_ALPHA>::prefix());
  if (iter != bstm_datas.end()) {
    alpha = iter->second;
  } else {
    return;
  }

  // get appearance
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

void convert_bstm_space_trees(bstm_multi_block *blk,
                              bstm_block *bstm_blk,
                              bstm_time_block *bstm_blk_t,
                              int current_level,
                              const bstm_data_base *alpha,
                              block_data_base *alpha_new,
                              const bstm_data_base *appearance,
                              block_data_base *appearance_new,
                              const vcl_string &appearance_type) {
  vcl_size_t app_type_size = bstm_data_info::datasize(appearance_type);
  vcl_size_t alpha_type_size = bstm_data_traits<BSTM_ALPHA>::datasize();
  vcl_size_t num_time_trees = bstm_blk_t->sub_block_num();
  vcl_size_t num_time_bytes = bstm_blk_t->byte_count();
  // Create new time buffer
  blk->new_buffer(num_time_bytes, current_level);
  unsigned char *new_time_buffer = blk->get_data(current_level);
  // Create new space buffer
  vcl_size_t num_space_bytes = bstm_blk->byte_count() * num_time_trees;
  blk->new_buffer(num_space_bytes, current_level - 1);
  unsigned char *new_space_buffer = blk->get_data(current_level);

  // Create copies of BSTM data buffers
  alpha_new->new_data_buffer(alpha->buffer_length());
  appearance_new->new_data_buffer(appearance->buffer_length());

  // Wrap new space block in 4D array so we can iterate over trees in row-major
  // order.
  space_tree_b *space_buffer_ptr =
      reinterpret_cast<space_tree_b *>(new_space_buffer);
  index_4d space_block_dims(bstm_blk->sub_block_num().x(),
                            bstm_blk->sub_block_num().y(),
                            bstm_blk->sub_block_num().z(),
                            num_time_trees);
  boxm2_array_3d<space_tree_b> &trees = bstm_blk->trees();
  boxm2_array_3d<space_tree_b>::iterator tree_iter = trees.begin();
  array_4d<space_tree_b> new_trees(space_buffer_ptr, space_block_dims);
  array_4d<space_tree_b>::iterator new_tree_iter = new_trees.begin();
  time_tree_b *time_buffer_ptr =
      reinterpret_cast<time_tree_b *>(new_time_buffer);

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
      boxm2_array_1d<time_tree_b> cell_time_trees = bstm_blk_t->get_cell_all_tt(
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

#define TIME_DIFFERENCES_INSTANTIATE(APP_TYPE)                                 \
  return time_differences_from_bstm_trees<APP_TYPE>(                           \
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

vcl_vector<bool> dispatch_time_differences_from_bstm_trees(
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

template <bstm_data_type APP_TYPE>
vcl_vector<bool> time_differences_from_bstm_trees(
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
  array_4d<space_tree_b> space_trees_array(
      reinterpret_cast<space_tree_b *>(space_buffer),
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

bool make_unrefined_space_tree(
    boct_bit_tree &current_tree,
    const vcl_pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const index_4d &coords,
    unsigned char *child_level_buffer,
    space_time_enum child_level_type,
    const vcl_vector<bool> &time_differences_vec) {
  // whether the whole space region is different from the previous frame.
  bool region_different = false;
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
            child_indices.index_from_coords(coords[0] * 8 + i_sub,
                                            coords[1] * 8 + j_sub,
                                            coords[2] * 8 + k_sub,
                                            coords[3]);
        unsigned char *sub_tree_bytes =
            child_level_buffer + (sub_tree_index * tree_size(child_level_type));
        // check root bit. If not zero, we need to keep current
        // space tree refined.
        bool root_bit =
            generic_tree(sub_tree_bytes, child_level_type).root_bit();
        // we want this to be the case some times. Fail so I can notice that
        // it's possible.
        assert(!root_bit);
        // Set corresponding leaf node on our main tree
        if (root_bit) {
          vcl_size_t leaf_index =
              leaf_node_indices.linear_index(i_sub / 2, j_sub / 2, k_sub / 2);
          current_tree.set_bit_and_parents_to_true(leaf_index);
        }
        if (time_differences_vec[sub_tree_index]) {
          region_different = true;
        }
      }
    }
  }
  return region_different;
}

// Since space-time regions are stored in row-major order, iterating
// over time is very simple - the same region in space has all its
// regions over time stored contiguously.
void make_unrefined_time_tree(bstm_time_tree &current_tree,
                              vcl_size_t tree_index,
                              const vcl_vector<bool> &time_differences_vec) {
  // stores true if frame is different than the last, false otherwise.
  bool frame_data[32];
  bool any_frames_different = false;
  // this time tree covers 32 "frames" from the lower level.
  for (vcl_size_t t_sub = 0; t_sub < 32; ++t_sub) {
    vcl_size_t sub_tree_index = tree_index * 32 + t_sub;
    frame_data[t_sub] = time_differences_vec[sub_tree_index];
    any_frames_different |= frame_data[t_sub];
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
  // Keeps track of the number of space/time regions at each level
  // This starts out equal to the number of blocks in the BSTM block, but one
  // level up.
  vcl_pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
      bstm_blk->sub_block_num(), bstm_blk_t->sub_block_num());
  if (volume(num_regions) == 0) {
    vcl_cerr << "Scene has 0 volume, i.e. scene is empty, doing nothing."
             << vcl_endl;
    return true;
  }

  // the first two levels are BSTM data -- need to rearrange them into row-major
  // order.
  int num_levels = blk->buffers().size();
  int current_level = num_levels - 1;
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
  // get per-frame differences over time of every voxel at lowest
  // level. This will be used to hierarchially coaelsce the scene over
  // time.
  vcl_vector<bool> time_differences_vec =
      dispatch_time_differences_from_bstm_trees(
          blk->get_data(current_level + 2),
          blk->get_data(current_level + 1),
          num_regions,
          datas["BSTM_ALPHA"],
          datas[appearance_type],
          appearance_type,
          p_threshold,
          app_threshold);
  if (time_differences_vec.size() == 0) {
    vcl_cerr << "Invalid appearance type " << appearance_type << "."
             << vcl_endl;
    return false;
  }

  // moving upwards through levels:
  for (; current_level >= 0; --current_level) {
    space_time_enum current_level_type = blk->level_type(current_level);
    space_time_enum child_level_type = blk->level_type(current_level + 1);
    unsigned char *child_level_buffer = blk->get_data(current_level + 1);
    // find proper refinement for this level
    switch (current_level_type) {
    case STE_SPACE: {
      // Create new empty buffer for this level
      num_regions.first /= 8;
      vcl_size_t num_trees = volume(num_regions);
      blk->new_buffer(num_trees * space_tree_size, current_level);
      space_tree_b *current_buffer =
          reinterpret_cast<space_tree_b *>(blk->get_data(current_level));
      vcl_memset(current_buffer, 0, num_trees * space_tree_size);
      array_4d<space_tree_b> trees(current_buffer,
                                   num_regions.first.x(),
                                   num_regions.first.y(),
                                   num_regions.first.z(),
                                   num_regions.second);
      vcl_vector<bool> time_differences_coalesced(trees.size(), false);
      for (int i = 0; i < trees.size(); ++i) {
        boct_bit_tree current_tree(trees[i]);
        // Region is different from previous frame if any of the subtrees is
        // different from previous frame.
        time_differences_coalesced[i] =
            make_unrefined_space_tree(current_tree,
                                      num_regions,
                                      trees.coords_from_index(i),
                                      child_level_buffer,
                                      child_level_type,
                                      time_differences_vec);
      }
      time_differences_vec.swap(time_differences_coalesced);
      break;
    }
    case STE_TIME: {
      num_regions.second /= 32;
      vcl_size_t num_trees = volume(num_regions);
      blk->new_buffer(num_trees * time_tree_size, current_level);
      time_tree_b *current_buffer =
          reinterpret_cast<time_tree_b *>(blk->get_data(current_level));
      vcl_memset(current_buffer, 0, num_trees * time_tree_size);
      array_4d<time_tree_b> trees(current_buffer,
                                  num_regions.first.x(),
                                  num_regions.first.y(),
                                  num_regions.first.z(),
                                  num_regions.second);
      vcl_vector<bool> time_differences_coalesced(trees.size(), false);
      for (int i = 0; i < trees.size(); ++i) {
        bstm_time_tree current_tree(trees[i]);
        make_unrefined_time_tree(current_tree, i, time_differences_vec);
        // This region is different than the previous one in two cases:
        // - 1. The time tree is refined at all
        // - 2. The time tree is unrefined (all frames are the same),
        //      but the first frame is different. This means it
        //      different from the *previous* region at this level.
        time_differences_coalesced[i] =
            current_tree.bit_at(0) || time_differences_vec[i];
      }
      time_differences_vec.swap(time_differences_coalesced);
      break;
    }
    }
  }

  // TODO currently, top level should only have one tree
  // this is just for debug
  assert(volume(num_regions) == 1);

  // XXX TODO last two levels (i.e. BSTM levels are not necessarily organized
  // this way)
  for (int level = 0; level < num_levels - 2; ++level) {
    space_time_enum level_type = blk->level_type(level);
    space_time_enum child_level_type = blk->level_type(level + 1);
    vcl_size_t t_size = tree_size(level_type);
    vcl_size_t child_t_size = tree_size(child_level_type);
    vcl_pair<vgl_vector_3d<unsigned>, unsigned> child_num_regions = num_regions;
    switch (level_type) {
    case STE_SPACE:
      child_num_regions.first *= 8;
    case STE_TIME:
      child_num_regions.second *= 32;
    }
    array_4d<int> current_level_array(VXL_NULLPTR, num_regions);
    array_4d<int> child_level_array(VXL_NULLPTR, child_num_regions);
    vcl_vector<unsigned char> &buffer = blk->get_buffer(level);
    vcl_vector<unsigned char> &child_buffer = blk->get_buffer(level + 1);
    vcl_vector<unsigned char> new_child_buffer = vcl_vector<unsigned char>();
    vcl_size_t num_trees = buffer.size() / t_size;

    int child_offset = 0;
    for (int tree_idx = 0; tree_idx < num_trees; ++tree_idx) {
      generic_tree tree(&buffer[0] + (tree_idx * t_size), level_type);
      // For first level, trees are already in proper order
      int tree_buffer_idx = tree_idx;
      // For other levels, the tree's index in the old (row-major order) buffer
      // is stored in data_ptr;
      if (level > 0) {
        tree_buffer_idx = tree.get_data_ptr();
      }
      index_4d tree_coords =
          current_level_array.coords_from_index(tree_buffer_idx);
      vcl_vector<int> leaf_indices = tree.get_leaf_bits();
      for (int leaf_idx = 0; leaf_idx < leaf_indices.size(); ++leaf_idx) {
        index_4d local_coords = tree.local_leaf_coords(leaf_idx);
        index_4d child_coords =
            element_product(tree_coords, tree.dimensions()) + local_coords;
        int child_index = child_level_array.index_from_coords(child_coords);
        unsigned char *child_tree_ptr =
            &child_buffer[0] + child_index * child_t_size;
        // If the child level is the original BSTM space trees, they already
        // have correct data pointers.
        if (level < num_levels - 3) {
          generic_tree(child_tree_ptr, child_level_type)
              .set_data_ptr(child_index);
        }
        new_child_buffer.insert(new_child_buffer.end(),
                                child_tree_ptr,
                                child_tree_ptr + child_t_size);
      }
      tree.set_data_ptr(child_offset);
      child_offset += leaf_indices.size();
    }
    child_buffer.swap(new_child_buffer);
    num_regions = child_num_regions;
  }

  {
    // Special case for penultimate level -- the original BSTM space trees
    vcl_vector<unsigned char> &space_buffer = blk->get_buffer(num_levels - 2);
    vcl_vector<unsigned char> &time_buffer = blk->get_buffer(num_levels - 1);
    vcl_vector<unsigned char> new_time_buffer;
    space_tree_b *space_trees =
        reinterpret_cast<space_tree_b *>(&space_buffer[0]);
    unsigned char *old_time_tree_ptr = &time_buffer[0];
    vcl_size_t num_trees = space_buffer.size() / space_tree_size;
    vcl_size_t num_data_elements = 0;
    for (vcl_size_t tree_idx = 0; tree_idx < num_trees; ++tree_idx) {
      boct_bit_tree current_space_tree(space_trees[tree_idx]);
      int num_leaves = current_space_tree.num_leaves();
      int data_ptr = current_space_tree.get_data_ptr();
      unsigned char *start = old_time_tree_ptr + data_ptr * time_tree_size;
      new_time_buffer.insert(
          new_time_buffer.end(), start, start + num_leaves * time_tree_size);
      current_space_tree.set_data_ptr(num_data_elements);
      num_data_elements += num_leaves;
    }
  }

  // we know the bottom level contains time trees, since we imported a BSTM
  // block
  // first, calculate size of data buffers
  // TODO we only need this because it's inconvenient to resize the data
  // buffers.
  {
    vcl_vector<unsigned char> &bottom_buffer = blk->get_buffer(num_levels - 1);
    time_tree_b *time_trees =
        reinterpret_cast<time_tree_b *>(&bottom_buffer[0]);
    vcl_size_t num_trees = bottom_buffer.size() / time_tree_size;
    vcl_size_t num_data_elements = 0;
    for (vcl_size_t tree_idx = 0; tree_idx < num_trees; ++tree_idx) {
      bstm_time_tree current_time_tree(time_trees[tree_idx]);
      num_data_elements += current_time_tree.num_leaves();
    }

    vcl_size_t app_type_size = bstm_data_info::datasize(appearance_type);
    vcl_size_t alpha_type_size = bstm_data_traits<BSTM_ALPHA>::datasize();
    block_data_base *new_alpha =
        new block_data_base(num_data_elements * alpha_type_size);
    block_data_base *new_appearance =
        new block_data_base(num_data_elements * app_type_size);

    // Now actually copy over data
    // BSTM data has its own data pointers into existing data array, so we can
    // use
    // those
    int new_data_ptr = 0;
    for (vcl_size_t tree_idx = 0; tree_idx < num_trees; ++tree_idx) {
      bstm_time_tree current_time_tree(time_trees[tree_idx]);
      int num_leaves = current_time_tree.num_leaves();
      int old_data_ptr = current_time_tree.get_data_ptr();
      vcl_memcpy(new_alpha->data_buffer() + new_data_ptr * alpha_type_size,
                 alpha->data_buffer() + old_data_ptr * alpha_type_size,
                 num_leaves * alpha_type_size);
      vcl_memcpy(new_appearance->data_buffer() + new_data_ptr * app_type_size,
                 appearance->data_buffer() + old_data_ptr * app_type_size,
                 num_leaves * app_type_size);
      // set data ptr
      current_time_tree.set_data_ptr(new_data_ptr);
      new_data_ptr += num_leaves;
    }
  }
  // We're done!
  return true;
}
