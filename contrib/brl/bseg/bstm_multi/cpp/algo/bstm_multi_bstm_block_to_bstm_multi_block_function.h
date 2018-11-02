#ifndef bstm_multi_bstm_block_to_bstm_multi_block_function_h_
#define bstm_multi_bstm_block_to_bstm_multi_block_function_h_

#include <iostream>
#include <map>
#include <stack>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bstm/bstm_block.h>
#include <bstm/bstm_block_metadata.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_data_traits.h>
#include <bstm/bstm_util.h>
#include <bstm/cpp/algo/bstm_data_similarity_traits.h>

#include <bstm_multi/basic/array_4d.h>
#include <bstm_multi/bstm_multi_block_metadata.h>
#include <bstm_multi/bstm_multi_typedefs.h>
#include <bstm_multi/space_time_scene.h>

//: \brief Returns the volume of a 4D region given as a vector of
// <x,y,z> coordinates and a time length.
//
// This is a just product of all the given dimension.
template <typename T> T volume(const std::pair<vgl_vector_3d<T>, T> &v) {
  return v.first.x() * v.first.y() * v.first.z() * v.second;
}

//: \brief Gets an appearance and an alpha model from a collection of
// data buffers. If there are multiple appearance models, only the
// first one is used. The buffers are stored in-place in 'alpha' and
// 'appearance', respectively.
//
// \param bstm_datas - map of data type prefix to data buffer
//
// \param alpha - pointer to occupancy data buffer. This value is modified by
// this function to point to the appropriate data buffer in `bstm_datas`.
//
// \param appearance - pointer to appearance data buffer. This value
// is modified by this function to point to the appropriate data
// buffer in `bstm_datas`.
//
// \param appearance_type - appearance data type as string
void get_bstm_data_buffers(
    const std::map<std::string, bstm_data_base *> &bstm_datas,
    bstm_data_base *&alpha,
    bstm_data_base *&appearance,
    std::string &appearance_type);

//: \brief Imports space and time trees from a BSTM block into a multi-BSTM
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
                              int current_level,
                              const bstm_data_base *alpha,
                              block_data_base *alpha_new,
                              const bstm_data_base *appearance,
                              block_data_base *appearance_new,
                              const std::string &appearance_type);

//: \brief convenience function for choosing a template specialization
// (based on appearance data type) at run-time using the appearance
// type string.
//
// \returns output of time_differences_from_bstm_trees(...) if data
// type is valid. If not, returns an empty std::vector<bool>.
std::vector<bool> dispatch_time_differences_from_bstm_trees(
    time_tree_b *time_buffer,
    space_tree_b *space_buffer,
    const std::pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const block_data_base *alpha,
    const block_data_base *appearance,
    const std::string &appearance_type,
    double p_threshold,
    double app_threshold);

// : \brief Given BSTM data, creates a 4D array of booleans that
// determines, for each region of space (corresponding to a space tree
// in BSTM), whether that region is the same as the corresponding
// region of space in the previous time frame.  This array is used to
// further coalesce the BSTM scene over time during the creation of
// the multi-BSTM scene. Two time frames are similar if they are a)
// totally unrefined or b) composed of one voxel who's appearance and
// occupancy data changes less than the given threshold over those two
// time frames.
//
// NOTE: By "BSTM data", it means actually output from
// convert_bstm_space_trees which is in row-major order over all 4
// dimensions, not raw BSTM data which is differently formatted.
//
// \tparam APP_TYPE - appearance data type.
//
// \param time_buffer - BSTM time trees, arranged in row major order
// over (x,y,z,t).
//
// \param space_buffer - BSTM space trees, arranged in row major order over
// (x,y,z,t). Note that each voxel of a space tree points to exactly one time
// tree. That is, instead of directly using BSTM space trees, use output from
// convert_bstm_space_trees. Essentially, if the scene is too long over time to
// store in one time tree, there should be one space tree for each 32-frame time
// region.
//
// \param num_regions - Number of space trees in each x,y,z,t dimension.
// \param alpha - Buffer of occupancy data
// \param app_threshold - Buffer of appearance data
//
// \returns A vector of booleans where each corresponds to a space
// tree, arranged in row-major order. The boolean representing region
// (x,y,z,t) is true if this region is significantly different from
// (x,y,z,t-1) and false otherwise.
template <bstm_data_type APP_TYPE>
std::vector<bool> time_differences_from_bstm_trees(
    time_tree_b *time_buffer,
    space_tree_b *space_buffer,
    const std::pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const block_data<BSTM_ALPHA> &alpha,
    const block_data<APP_TYPE> &appearance,
    double p_threshold,
    double app_threshold) {
  typedef typename bstm_data_traits<BSTM_ALPHA>::datatype alpha_data_type;
  typedef typename bstm_data_traits<APP_TYPE>::datatype app_data_type;

  // used for converting between 4D coordinates and array indices
  array_4d<space_tree_b> space_trees_array(space_buffer,
                                           num_regions.first.x(),
                                           num_regions.first.y(),
                                           num_regions.first.z(),
                                           num_regions.second);
  std::vector<bool> time_differences(space_trees_array.size(), false);
  for (std::size_t i = 0; i < space_trees_array.x(); ++i) {
    for (std::size_t j = 0; j < space_trees_array.y(); ++j) {
      for (std::size_t k = 0; k < space_trees_array.z(); ++k) {
        // arrays are stored in row-major order, so we can directly iterate over
        // time dimension.
        std::size_t first_frame_idx =
            space_trees_array.index_from_coords(i, j, k, 0);
        boct_bit_tree last_same_tree(space_trees_array[first_frame_idx]);
        for (std::size_t idx = first_frame_idx;
             idx < first_frame_idx + num_regions.second;
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
          std::vector<int> leaf_bits = current_tree.get_leaf_bits();
          for (std::vector<int>::const_iterator iter = leaf_bits.begin();
               iter != leaf_bits.end();
               ++iter) {
            std::size_t current_tt_idx = current_tree.get_data_index(*iter);
            std::size_t previous_tt_idx = last_same_tree.get_data_index(*iter);
            bstm_time_tree current_tt(time_buffer[current_tt_idx]);
            bstm_time_tree previous_tt(time_buffer[previous_tt_idx]);
            // If time trees are refined at all, then they have more than one
            // frame and are not a single identical region. Do not refine.
            if (current_tt.bit_at(0) || previous_tt.bit_at(0)) {
              time_differences[idx] = true;
              last_same_tree = current_tree;
              break;
            }
            // Now compare actual data values
            std::size_t current_data_idx = current_tt.get_data_index(0);
            std::size_t previous_data_idx = previous_tt.get_data_index(0);
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

//: \brief computes structure of a space tree given underlying buffer.
//
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
//
// \param time_differences_vec - vector showing whether each region in
// space-time is different from the previous on in time. This needs to
// be coalesced over space to accurately represent the new, unrefined
// tree.
bool make_unrefined_space_tree(
    boct_bit_tree &current_tree,
    const std::pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    const index_4d &coords,
    unsigned char *child_level_buffer,
    space_time_enum child_level_type,
    const std::vector<bool> &time_differences_vec);

//: \brief Similar to make_unrefined_space_tree, except coalesces over
// time. The difference here is that instead of coalescing based on
// whether sub-regions are refined or not, this coalesces based on
// similarity over time.
//
// \param current_tree - time tree that represents the region in
// question. This is filled with the appropriate bits.
//
// \param tree_index - index of the tree in the list of all trees at
// this current level
//
// \param time_differences_vec - whether each region in space-time is
// similar to the same region in the previous frame.
void make_unrefined_time_tree(bstm_time_tree &current_tree,
                              std::size_t tree_index,
                              const std::vector<bool> &time_differences_vec);

//: Data structure for representing a node in a bstm_multi_block. This is
// returned by iterators traversing the block's data.
struct bstm_multi_block_tree_node {
  const generic_tree tree;
  index_4d coords;
  int level;
};

//: \brief Computes the structure of trees at higher levels of a multi-BSTM
// scene given BSTM data.
//
// Creates buffers of space or time trees arranged in row-major order
// that each represent a region of space-time in the undelrying buffer
// of the level below. Each level will have a fixed number of trees
// based on the structure of the scene, and there will be many
// "oprhan" trees at various levels that are not pointed to by any
// trees at higher levels. This is cleaned up in coalesce_trees.
//
// Assumes that bottom two levels contain output from
// convert_bstm_trees. Only modifies the levels above those two.
//
// \param blk - The given Multi-BSTM scene
// \param num_regions - The number of BSTM space trees in each dimension.
// \param time_differences_vec - A row-major array of booleans that stores, for
// each space time region (cooresponding to a single BSTM space tree), whether
// that region is different in the previous frame / time interval. This array is
// coalesced and resized as this function progresses through each level.
void compute_trees_structure(
    bstm_multi_block *blk,
    std::pair<vgl_vector_3d<unsigned>, unsigned> &num_regions,
    std::vector<bool> &time_differences_vec);

//: \brief Rearranges trees & data in a multi-BSTM scene so that each
// tree's children are contiguous in memory and so that elements not
// pointed to by trees are removed.
//
// This takes a scene in which every level except the last two has its
// trees in row-major (x,y,z,t) order. The last two levels are
// imported from a BSTM scene, and thus should be in the same format
// as if they were produced by convert_bstm_space_trees(...). Data
// pointers in the non-BSTM levels do not need to be correct, as they
// are created by this function.
//
// \param blk - Multi-BSTM block. The buffers in this block are
// updated with the rearranged data. The old memory is deleted.
//
// \param datas - Data buffers for this scene. The "BSTM_ALPHA" and
// appearance_type buffers are updated by this function. The old
// buffers are deleted.
//
// \param num_regions - Represents number of blocks in each dimension
// at top level. TODO: right now assumed to be one, so a) allow
// multiple top-level blocks and b) create member function
// bstm_multi_block::get_num_regions().
//
// \param appearance_type - Appearance data type
void coalesce_trees(bstm_multi_block *blk,
                    std::map<std::string, block_data_base *> &datas,
                    std::pair<vgl_vector_3d<unsigned>, unsigned> num_regions,
                    const std::string &appearance_type);

bool bstm_block_to_bstm_multi_block(
    bstm_multi_block *blk,
    std::map<std::string, block_data_base *> &datas,
    bstm_block *bstm_blk,
    bstm_time_block *bstm_blk_t,
    const std::map<std::string, bstm_data_base *> &bstm_datas,
    double p_threshold,
    double app_threshold);

#endif // bstm_multi_bstm_to_bstm_multi_scene_function_h_
