//:
// \file test_bstm_to_multi_bstm_block_function.cxx
// \brief Tests BSTM to Multi-BSTM conversion function
// \author Raphael Kargon
// \date 21-Aug-2017

#include <iostream>
#include <string>
#include <utility>
#include <testlib/testlib_root_dir.h>
#include <testlib/testlib_test.h>

#include <vbl/vbl_smart_ptr.hxx>
#include <vbl/vbl_triple.hxx>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_vector_3d.h>

#include <bstm/bstm_data.h>

#include <bstm_multi/cpp/algo/bstm_multi_bstm_block_to_bstm_multi_block_function.h>

/* A function for creating a BSTM scene, given a function that
   represents the data for each voxel. Useful for creting artifical
   BSTM Scenes for testing.

   TODO: doesn't quite match up with the results from test_fly_problem
   (different numbers of data elements, although tree counts are the
   same.) so may not be fully correct.

   Also, this is rather slow. It might help to have another function
   to specify whether a space tree is entirely empty so we can skip
   it.
 */

template <bstm_data_type ALPHA, bstm_data_type APPEARANCE>
struct voxel_functor {
  typedef std::pair<typename bstm_data_traits<ALPHA>::datatype,
                   typename bstm_data_traits<APPEARANCE>::datatype> (
      *fptr_type)(const index_4d &);
};

template <bstm_data_type ALPHA, bstm_data_type APPEARANCE>
vbl_triple<bstm_block *,
           bstm_time_block *,
           std::map<std::string, bstm_data_base *> >
bstm_scene_from_point_func(
    const bstm_block_metadata &mdata,
    typename voxel_functor<ALPHA, APPEARANCE>::fptr_type f,
    double p_threshold = 0.01,
    double app_threshold = 0.01) {
  typedef typename bstm_data_traits<ALPHA>::datatype alpha_t;
  typedef typename bstm_data_traits<APPEARANCE>::datatype appearance_t;

  bstm_block *blk = new bstm_block(mdata);
  std::vector<time_tree_b> time_trees;
  std::vector<alpha_t> alpha;
  std::vector<appearance_t> appearance;
  boxm2_array_3d<space_tree_b> &space_trees = blk->trees();
  int num_time_intervals = mdata.sub_block_num_t_;
  for (int space_idx = 0; space_idx < space_trees.size(); ++space_idx) {
    std::size_t x, y, z;
    space_trees.coords_from_index(space_idx, x, y, z);
    boct_bit_tree current_tree(space_trees(x, y, z));
    current_tree.set_data_ptr(time_trees.size() / num_time_intervals);

    // refine bits of current space tree
    for (int st_x = 0; st_x < 8; ++st_x) {
      for (int st_y = 0; st_y < 8; ++st_y) {
        for (int st_z = 0; st_z < 8; ++st_z) {
          int space_cell_idx = current_tree.traverse(
              vgl_point_3d<double>(st_x / 8.0, st_y / 8.0, st_z / 8.0),
              4,
              true);
          for (int t = 0; t < num_time_intervals * 32; ++t) {
            std::pair<alpha_t, appearance_t> voxel_data =
                f(index_4d(x * 8 + st_x, y * 8 + st_y, z * 8 + st_z, t));
            if (voxel_data.first > 0) {
              current_tree.set_bit_and_parents_to_true(
                  current_tree.parent_index(space_cell_idx));
              break;
            }
          }
        }
      }
    }

    std::vector<int> cells = current_tree.get_cell_bits();
    for (int cell_idx : cells) {
      if (cell_idx >= 73) {
        vgl_point_3d<double> p = current_tree.cell_box(cell_idx).min_point();
        int st_x = p.x() * 8;
        int st_y = p.y() * 8;
        int st_z = p.z() * 8;
        for (int tt_idx = 0; tt_idx < num_time_intervals; ++tt_idx) {
          // initialize time tree
          time_trees.emplace_back();
          bstm_time_tree tt(time_trees.back());
          tt.set_data_ptr(alpha.size());

          bool diffs[32]; // whether each frame is different from the last
          alpha_t alpha_frames[32];
          appearance_t appearance_frames[32];
          // get data for each frame
          for (int t = 0; t < 32; ++t) {
            // TODO find clean way to do this w/o calling f twice.
            std::pair<alpha_t, appearance_t> voxel_data = f(index_4d(
                x * 8 + st_x, y * 8 + st_y, z * 8 + st_z, 32 * tt_idx + t));
            alpha_frames[t] = voxel_data.first;
            appearance_frames[t] = voxel_data.second;
            if (t > 0) {
              diffs[t] = !is_similar<APPEARANCE>(appearance_frames[t - 1],
                                                 appearance_frames[t],
                                                 alpha_frames[t - 1],
                                                 alpha_frames[t],
                                                 p_threshold,
                                                 app_threshold);
            }
          }
          tt.fill_cells(diffs);
          std::vector<int> tt_leaves = tt.get_leaf_bits();
          for (std::vector<int>::const_iterator tt_leaf_iter = tt_leaves.begin();
               tt_leaf_iter != tt_leaves.end();
               ++tt_leaf_iter) {
            if (*tt_leaf_iter < 31) {
              alpha.push_back(alpha_t(0));
              appearance.push_back(appearance_t((unsigned char)0));
            } else {
              int t = *tt_leaf_iter - 31;
              alpha.push_back(alpha_frames[t]);
              appearance.push_back(appearance_frames[t]);
            }
          }
        }
      } else {
        // add empty time trees and data elements to this non-leaf cell
        for (int tt_idx = 0; tt_idx < num_time_intervals; ++tt_idx) {
          time_trees.emplace_back((unsigned char)0);
          bstm_time_tree(time_trees.back()).set_data_ptr(alpha.size());
          alpha.push_back(alpha_t(0));
          appearance.push_back(appearance_t((unsigned char)0));
        }
      }
    }
  }

  std::size_t time_buff_size = time_trees.size() * time_tree_size;
  char *time_buffer = new char[time_buff_size];
  std::memcpy(time_buffer, &(time_trees[0]), time_buff_size);
  bstm_time_block *blk_t =
      new bstm_time_block(bstm_block_id(), mdata, time_buffer, time_buff_size);

  std::size_t alpha_buff_size =
      alpha.size() * bstm_data_traits<BSTM_ALPHA>::datasize();
  char *alpha_buffer = new char[alpha_buff_size];
  std::memcpy(alpha_buffer, &(alpha[0]), alpha_buff_size);

  std::size_t appearance_buff_size =
      appearance.size() * bstm_data_traits<BSTM_MOG3_GREY>::datasize();
  char *appearance_buffer = new char[appearance_buff_size];
  std::memcpy(appearance_buffer, &(appearance[0]), appearance_buff_size);

  std::map<std::string, bstm_data_base *> datas;

  datas["alpha"] =
      new bstm_data_base(alpha_buffer, alpha_buff_size, blk->block_id());
  datas["bstm_mog3_grey"] = new bstm_data_base(
      appearance_buffer, appearance_buff_size, blk->block_id());

  return vbl_make_triple(blk, blk_t, datas);
}

void test_volume() {
  std::pair<vgl_vector_3d<int>, int> region(vgl_vector_3d<int>(2, 3, 4), 5);
  TEST("test volume int", volume(region), 120);
  std::pair<vgl_vector_3d<double>, double> region2(
      vgl_vector_3d<double>(10, 10, 10), 10);
  TEST("test volume double", volume(region2), 1e4);
}

void test_get_bstm_data_buffers() {
  bstm_data_base *alpha = nullptr, *app = nullptr;
  std::string app_type, alpha_prefix = bstm_data_traits<BSTM_ALPHA>::prefix();
  std::map<std::string, bstm_data_base *> map;
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, empty map", alpha, nullptr);
  TEST("get bstm data buffs, empty map", app, nullptr);

  map[alpha_prefix] = new bstm_data_base(nullptr, 0, bstm_block_id());
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, no app", alpha, map[alpha_prefix]);
  TEST("get bstm data buffs, no app", app, nullptr);

  map["BSTM_SCHMERZ"] = new bstm_data_base(nullptr, 0, bstm_block_id());
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, no matching app", alpha, map[alpha_prefix]);
  TEST("get bstm data buffs, no matching app", app, nullptr);

  map[bstm_data_traits<BSTM_MOG3_GREY>::prefix()] =
      new bstm_data_base(nullptr, 0, bstm_block_id());
  get_bstm_data_buffers(map, alpha, app, app_type);
  TEST("get bstm data buffs, has app", alpha, map[alpha_prefix]);
  TEST("get bstm data buffs, has app",
       app,
       map[bstm_data_traits<BSTM_MOG3_GREY>::prefix()]);
}

// Essentially this test sets up a BSTM scene where each tree has one
// level of refinement. Then, all the data elemenst are set to a
// unique value based on their spatial coordinates in the scene. After
// convert_bstm_space_trees, the data elements should still be
// recoverable based on their spatial coordinates, even though they've
// been internally rearranged.
void test_convert_bstm_space_trees() {
  /*  set up Multi-BSTM block  */
  std::vector<space_time_enum> subdivs(2);
  subdivs[0] = STE_SPACE;
  subdivs[1] = STE_TIME;
  bstm_multi_block_metadata mdata(bstm_block_id(),
                                  vgl_box_3d<double>(0, 0, 0, 1, 1, 1),
                                  std::pair<double, double>(0, 1),
                                  9000,
                                  0.01,
                                  subdivs);
  bstm_multi_block_sptr blk = new bstm_multi_block(mdata);

  /* Set up bstm blocks */
  bstm_block_metadata bstm_mdata(bstm_block_id(),
                                 vgl_point_3d<double>(0, 0, 0),
                                 0,
                                 vgl_vector_3d<double>(.1, .1, .1),
                                 .1,
                                 vgl_vector_3d<unsigned>(10, 10, 10),
                                 10,
                                 1,
                                 4,
                                 9000,
                                 .01);

  int num_space_trees = 10 * 10 * 10;
  // 10 time intervals, and each space tree will have 8 leaves (and one root
  // node)
  int num_time_trees = num_space_trees * 10 * 9;
  // each time tree will have two leaves
  int num_data_elements = num_time_trees * 2;

  char *bstm_buff = new char[num_space_trees * space_tree_size]();
  bstm_block_sptr bstm_blk =
      new bstm_block(bstm_mdata.id(), bstm_mdata, bstm_buff);
  bstm_time_block_sptr bstm_blk_t =
      new bstm_time_block(bstm_block_id(), bstm_mdata, num_time_trees);

  /* Set up data buffers */
  std::size_t alpha_size = bstm_data_traits<BSTM_ALPHA>::datasize();
  std::size_t app_size = bstm_data_traits<BSTM_MOG3_GREY>::datasize();
  bstm_data_base_sptr alpha =
      new bstm_data_base(num_data_elements, "alpha", bstm_mdata.id());
  block_data_base_sptr alpha_new = new block_data_base(0);
  std::string appearance_type = bstm_data_traits<BSTM_MOG3_GREY>::prefix();
  bstm_data_base_sptr appearance =
      new bstm_data_base(num_data_elements, appearance_type, bstm_mdata.id());
  block_data_base_sptr appearance_new = new block_data_base(0);

  /* Initialize data buffers */
  boxm2_array_3d<space_tree_b> &trees = bstm_blk->trees();
  array_4d<int> cell_indices_4d(nullptr, 10 * 8, 10 * 8, 10 * 8, 10 * 32);
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 10; ++j) {
      for (int k = 0; k < 10; ++k) {
        int idx = trees.linear_index(i, j, k);
        boct_bit_tree current_tree(trees(i, j, k));
        current_tree.set_bit_at(0, true);
        current_tree.set_data_ptr(idx * 9);
        std::vector<int> space_cells = current_tree.get_cell_bits();
        assert(space_cells.size() == 9);

        // Iterate over cells of space tree
        for (int cell_idx = 0; cell_idx < space_cells.size(); ++cell_idx) {
          int space_data_index =
              current_tree.get_data_index(space_cells[cell_idx]);

          // calc global coordinates of this leaf
          generic_tree gen_tree(current_tree);
          index_4d local_coords =
              gen_tree.local_voxel_coords(space_cells[cell_idx]);
          index_4d child_coords =
              element_product(index_4d(i, j, k, 0), gen_tree.dimensions()) +
              local_coords;

          boxm2_array_1d<time_tree_b> time_trees =
              bstm_blk_t->get_cell_all_tt(space_data_index);
          assert(time_trees.size() == 10);
          for (int t = 0; t < 10; ++t) {
            child_coords[3] = t * 32;
            std::size_t row_major_index =
                cell_indices_4d.index_from_coords(child_coords);

            // two data elements for each time tree
            int tt_idx = ((idx * 9 + cell_idx) * 10 + t);
            // there might be an overflow issue here :-(
            assert(tt_idx >= 0);
            assert(tt_idx * 2 >= 0);

            bstm_time_tree time_tree(time_trees[t]);
            time_tree.set_data_ptr(tt_idx * 2);
            time_tree.set_bit_at(0, true);

            std::vector<int> time_leaves = time_tree.get_leaf_bits();
            assert(time_leaves.size() == 2);
            for (int tl = 0; tl < time_leaves.size(); ++tl) {
              row_major_index += 16 * tl;
              std::size_t tt_data_index =
                  time_tree.get_data_index(time_leaves[tl]);
              reinterpret_cast<bstm_data_traits<BSTM_ALPHA>::datatype &>(
                  alpha->data_buffer()[tt_data_index * alpha_size]) =
                  row_major_index;
              reinterpret_cast<std::size_t &>(
                  appearance->data_buffer()[tt_data_index * app_size]) =
                  row_major_index;
            }
          }
        }
      }
    }
  }

  convert_bstm_space_trees(blk.ptr(),
                           bstm_blk.ptr(),
                           bstm_blk_t.ptr(),
                           1,
                           alpha.ptr(),
                           alpha_new.ptr(),
                           appearance.ptr(),
                           appearance_new.ptr(),
                           appearance_type);

  bstm_data<BSTM_ALPHA> alpha_wrap(*alpha);
  block_data<BSTM_ALPHA> alpha_new_wrap(*alpha_new);
  bstm_data<BSTM_MOG3_GREY> appearance_wrap(*appearance);
  block_data<BSTM_MOG3_GREY> appearance_new_wrap(*appearance_new);

  array_4d<space_tree_b> multi_bstm_space_trees(
      reinterpret_cast<space_tree_b *>(&(blk->get_buffer(0)[0])),
      10,
      10,
      10,
      10);
  array_4d<time_tree_b> multi_bstm_time_trees(
      reinterpret_cast<time_tree_b *>(&(blk->get_buffer(1)[0])),
      10,
      10,
      10,
      90);
  bool space_idxs_good = true, time_idxs_good = true, data_layout_good = true;
  for (int idx = 0; idx < multi_bstm_space_trees.size(); ++idx) {
    index_4d space_tree_coords = multi_bstm_space_trees.coords_from_index(idx);
    boct_bit_tree current_space_tree(multi_bstm_space_trees[idx]);
    generic_tree gen_tree(current_space_tree);
    space_idxs_good &= (current_space_tree.get_data_ptr() == idx * 9);
    for (int i = 0; i < 9; ++i) {
      bstm_time_tree current_time_tree(multi_bstm_time_trees[idx * 9 + i]);
      time_idxs_good &= (current_time_tree.get_data_ptr() == (idx * 9 + i) * 2);

      index_4d local_coords = gen_tree.local_voxel_coords(i);
      index_4d child_coords =
          element_product(space_tree_coords, gen_tree.dimensions()) +
          local_coords;
      child_coords[3] *= 32;

      // data value is the row-major idx of the current voxel
      std::size_t data_value = cell_indices_4d.index_from_coords(child_coords);
      for (int di = 0; di < 2; ++di) {
        data_layout_good &=
            (alpha_new_wrap[current_time_tree.get_data_ptr() + di] ==
             data_value + di * 16);
        data_layout_good &=
            (reinterpret_cast<std::size_t &>(
                 appearance_new_wrap[current_time_tree.get_data_ptr() + di]) ==
             data_value + di * 16);
      }
    }
  }
  TEST("convert_bstm_space_trees -- space tree data ptrs",
       space_idxs_good,
       true);
  TEST(
      "convert_bstm_space_trees -- time trees data ptrs", time_idxs_good, true);
  TEST("convert_bstm_space_trees -- data elements layout",
       data_layout_good,
       true);
}

void test_time_differences_from_bstm_trees() {
  std::pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
      vgl_vector_3d<unsigned>(3, 3, 3), 3);
  int num_space_trees = volume(num_regions);

  // test completely empty scene
  {
    int num_time_trees = num_space_trees * 9;
    int num_data_elements = num_time_trees;
    // initialize data buffers
    auto *space_buffer = new space_tree_b[num_space_trees]();
    std::memset(space_buffer, 0, num_space_trees * space_tree_size);
    auto *time_buffer = new time_tree_b[num_time_trees]();
    std::memset(time_buffer, 0, num_time_trees * time_tree_size);
    block_data_base alpha =
        block_data_base(num_data_elements, std::string("alpha"));
    block_data<BSTM_ALPHA> alpha_wrap(alpha);
    std::string appearance_type = "bstm_mog3_grey";
    block_data_base appearance =
        block_data_base(num_data_elements, appearance_type);
    block_data<BSTM_MOG3_GREY> appearance_wrap(appearance);

    // initialize tree structure - each tree has only root bit set
    for (int i = 0; i < num_space_trees; ++i) {
      boct_bit_tree space_tree(space_buffer[i]);
      space_tree.set_bit_at(0, true);
      space_tree.set_data_ptr(i * 9);
    }
    for (int i = 0; i < num_time_trees; ++i) {
      bstm_time_tree time_tree(time_buffer[i]);
      time_tree.set_data_ptr(i);
    }

    std::vector<bool> diffs =
        dispatch_time_differences_from_bstm_trees(time_buffer,
                                                  space_buffer,
                                                  num_regions,
                                                  &alpha,
                                                  &appearance,
                                                  appearance_type,
                                                  0.01,
                                                  0.01);
    TEST("time differences result length", diffs.size(), num_space_trees);
    bool time_diff_any = false;
    for (auto && diff : diffs) {
      time_diff_any |= diff;
    }
    TEST("time differences - empty scene with unrefined time trees",
         time_diff_any,
         false);

    delete[] time_buffer;
    delete[] space_buffer;
  }

  // test scene with differently refined space trees
  {
    int num_time_trees = num_space_trees +
                         9 * 3; // First two space trees have 8 extra time trees
    int num_data_elements = num_time_trees;
    // initialize data buffers
    auto *space_buffer = new space_tree_b[num_space_trees]();
    std::memset(space_buffer, 0, num_space_trees * space_tree_size);
    auto *time_buffer = new time_tree_b[num_time_trees]();
    std::memset(time_buffer, 0, num_time_trees * time_tree_size);
    block_data_base alpha =
        block_data_base(num_data_elements, std::string("alpha"));
    block_data<BSTM_ALPHA> alpha_wrap(alpha);
    std::string appearance_type = "bstm_mog3_grey";
    block_data_base appearance =
        block_data_base(num_data_elements, appearance_type);
    block_data<BSTM_MOG3_GREY> appearance_wrap(appearance);

    // initialize tree structure - each tree has only root bit set
    int ptr = 0;
    for (int i = 0; i < num_space_trees; ++i) {
      boct_bit_tree space_tree(space_buffer[i]);
      space_tree.set_data_ptr(ptr);
      if (i < 2 || i == 4) {
        space_tree.set_bit_at(0, true);
        ptr += 9;
      } else {
        ptr++;
      }
    }
    for (int i = 0; i < num_time_trees; ++i) {
      bstm_time_tree time_tree(time_buffer[i]);
      time_tree.set_data_ptr(i);
    }

    std::vector<bool> diffs =
        dispatch_time_differences_from_bstm_trees(time_buffer,
                                                  space_buffer,
                                                  num_regions,
                                                  &alpha,
                                                  &appearance,
                                                  appearance_type,
                                                  0.01,
                                                  0.01);
    bool diffs_good = true;
    for (int i = 0; i < diffs.size(); ++i) {
      if (i == 2 || i == 4 || i == 5) {
        diffs_good &= (diffs[i] == true);
      } else {
        diffs_good &= (diffs[i] == false);
      }
    }
    TEST("time differences - different space tree refinements",
         diffs_good,
         true);

    delete[] time_buffer;
    delete[] space_buffer;
  }

  // test scene with refined times trees
  {
    int num_time_trees = num_space_trees * 9;
    int num_data_elements = num_time_trees * 2;
    // initialize data buffers
    auto *space_buffer = new space_tree_b[num_space_trees]();
    std::memset(space_buffer, 0, num_space_trees * space_tree_size);
    auto *time_buffer = new time_tree_b[num_time_trees]();
    std::memset(time_buffer, 0, num_time_trees * time_tree_size);
    block_data_base alpha =
        block_data_base(num_data_elements, std::string("alpha"));
    block_data<BSTM_ALPHA> alpha_wrap(alpha);
    std::string appearance_type = "bstm_mog3_grey";
    block_data_base appearance =
        block_data_base(num_data_elements, appearance_type);
    block_data<BSTM_MOG3_GREY> appearance_wrap(appearance);

    // initialize tree structure - each tree has only root bit set
    for (int i = 0; i < num_space_trees; ++i) {
      boct_bit_tree space_tree(space_buffer[i]);
      space_tree.set_bit_at(0, true);
      space_tree.set_data_ptr(i * 9);
    }
    for (int i = 0; i < num_time_trees; ++i) {
      bstm_time_tree time_tree(time_buffer[i]);
      time_tree.set_bit_at(0, true);
      time_tree.set_data_ptr(i * 2);
    }

    std::vector<bool> diffs =
        dispatch_time_differences_from_bstm_trees(time_buffer,
                                                  space_buffer,
                                                  num_regions,
                                                  &alpha,
                                                  &appearance,
                                                  appearance_type,
                                                  0.01,
                                                  0.01);
    bool time_diff_all = true;
    for (auto && diff : diffs) {
      time_diff_all &= diff;
    }
    TEST("time differences - all time trees refined", time_diff_all, true);

    delete[] time_buffer;
    delete[] space_buffer;
  }

  // test scene with different data values
  {
    int num_time_trees = num_space_trees * 9; // 8 leaf nodes and 1 inner node
    int num_data_elements = num_time_trees;
    // initialize data buffers
    auto *space_buffer = new space_tree_b[num_space_trees]();
    std::memset(space_buffer, 0, num_space_trees * space_tree_size);
    auto *time_buffer = new time_tree_b[num_time_trees]();
    std::memset(time_buffer, 0, num_time_trees * time_tree_size);
    block_data_base alpha =
        block_data_base(num_data_elements, std::string("alpha"));
    block_data<BSTM_ALPHA> alpha_wrap(alpha);
    std::string appearance_type = "bstm_mog3_grey";
    block_data_base appearance =
        block_data_base(num_data_elements, appearance_type);
    block_data<BSTM_MOG3_GREY> appearance_wrap(appearance);

    // initialize tree structure - each tree has only root bit set
    for (int i = 0; i < num_space_trees; ++i) {
      boct_bit_tree space_tree(space_buffer[i]);
      space_tree.set_bit_at(0, true);
      space_tree.set_data_ptr(i * 9);
    }
    for (int i = 0; i < num_time_trees; ++i) {
      bstm_time_tree time_tree(time_buffer[i]);
      time_tree.set_data_ptr(i);
    }

    // set up differences in alpha and appearance - first and last cells
    // should
    // be different
    alpha_wrap[1] = 1.0;
    alpha_wrap[alpha_wrap.size() - 1] = 1.0;
    std::memset(&(appearance_wrap[appearance_wrap.size() - 1]),
               128,
               bstm_data_traits<BSTM_MOG3_GREY>::datasize());

    std::vector<bool> diffs =
        dispatch_time_differences_from_bstm_trees(time_buffer,
                                                  space_buffer,
                                                  num_regions,
                                                  &alpha,
                                                  &appearance,
                                                  appearance_type,
                                                  0.01,
                                                  0.01);
    bool diffs_good = true;
    for (int i = 0; i < diffs.size(); ++i) {
      // only first and last cells should be different
      if (i == 1 || i == num_space_trees - 1) {
        diffs_good &= (diffs[i] == true);
      } else {
        diffs_good &= (diffs[i] == false);
      }
    }
    TEST("time differences - first and last data cells different",
         diffs_good,
         true);

    delete[] time_buffer;
    delete[] space_buffer;
  }
}

void test_make_unrefined_time_tree() {
  // This is a somewhat contrived test - this function is an
  // intermediate step and as such assumes data is arranged a certain
  // way.
  {
    bstm_time_tree t;
    std::vector<bool> diffs(32, false);
    make_unrefined_time_tree(t, 0, diffs);
    TEST("make unrefined time tree, all frames the same", t.num_leaves(), 1);
  }
  {
    bstm_time_tree t;
    // three sets of time trees, but 't' corresponds to the middle one,
    // index=1
    std::vector<bool> diffs(32 * 3, false);
    diffs[32 + 16] = true;
    make_unrefined_time_tree(t, 1, diffs);
    TEST("make unrefined time tree, two sets of frames split at middle",
         t.num_leaves(),
         2);
  }
  {
    bstm_time_tree t;
    // three sets of time trees, but 't' corresponds to the middle one,
    // index=1
    std::vector<bool> diffs(32 * 3, false);
    std::fill(diffs.begin(), diffs.end(), true);
    make_unrefined_time_tree(t, 1, diffs);
    TEST("make unrefined time tree, all frames different from the last",
         t.num_leaves(),
         32);
  }
}

void test_make_unrefined_space_tree() {
  {
    boct_bit_tree t;
    std::pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
        vgl_vector_3d<unsigned>(2, 2, 2), 2);
    int vol = volume(num_regions);
    index_4d coords(1, 0, 0, 1);
    auto *child_level_buffer =
        new unsigned char[vol * 512 * space_tree_size]();
    std::vector<bool> diffs(vol * 512, false);

    bool res = make_unrefined_space_tree(
        t, num_regions, coords, child_level_buffer, STE_SPACE, diffs);
    TEST("unrefine space tree, all children empty and same", res, false);
    TEST(
        "unrefine space tree, all children empty and same", t.bit_at(0), false);
    delete[] child_level_buffer;
  }

  {
    boct_bit_tree t = boct_bit_tree();
    std::pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
        vgl_vector_3d<unsigned>(2, 2, 2), 2);
    int vol = volume(num_regions);
    index_4d coords(1, 0, 0, 1);
    auto *child_level_buffer =
        new unsigned char[vol * 512 * space_tree_size]();
    std::vector<bool> diffs(vol * 512, false);

    // set one of the sub-trees to be different
    diffs[9 * 512 + 100 + 1] = true;
    unsigned char *current_children_start =
        child_level_buffer + (8 * 512 + 1) * space_tree_size;
    boct_bit_tree(current_children_start).set_bit_at(0, true);
    boct_bit_tree(current_children_start + space_tree_size * 2)
        .set_bit_at(0, true);

    // first two cells (in row major order) should be refined, as well as
    // their
    // parents ofc.

    bool res = make_unrefined_space_tree(
        t, num_regions, coords, child_level_buffer, STE_SPACE, diffs);
    TEST("unrefine space tree, some children different", res, true);
    int l1_idx =
        t.parent_index(t.traverse(vgl_point_3d<double>(0, 0, 0), 4, true));
    int l2_idx = t.parent_index(
        t.traverse(vgl_point_3d<double>(0, 0, 1 / 8.0), 4, true));
    // test that other low-level bits are unrefined
    bool refined = false;
    for (int i = 73; i < 584; ++i) {
      if (i != l1_idx && i != l2_idx) {
        refined |= t.bit_at(i);
      }
    }
    TEST("All but two bits are unrefined.", refined, false);
    // Test refined bits
    for (; l1_idx >= 0; l1_idx = t.parent_index(l1_idx)) {
      TEST("l1_idx", t.bit_at(l1_idx), true);
    }
    for (; l2_idx >= 0; l2_idx = t.parent_index(l2_idx)) {
      TEST("l2_idx", t.bit_at(l2_idx), true);
    }

    delete[] child_level_buffer;
  }
}

//: Tests compute_tree_structure and coalesce_trees for a sample scene, that
// is
// the output of convert_bstm_trees.
void test_compute_and_coalesce_trees() {
  std::vector<space_time_enum> subdivs(4);
  subdivs[0] = STE_SPACE;
  subdivs[1] = STE_TIME;
  subdivs[2] = STE_SPACE;
  subdivs[3] = STE_TIME;
  bstm_multi_block_metadata mdata(bstm_block_id(),
                                  vgl_box_3d<double>(0, 0, 0, 1, 1, 1),
                                  std::pair<double, double>(0, 1),
                                  9000,
                                  0.01,
                                  subdivs);
  std::pair<vgl_vector_3d<unsigned>, unsigned> num_regions(
      vgl_vector_3d<unsigned>(8, 8, 8), 32);
  bstm_multi_block_sptr blk = new bstm_multi_block(mdata);

  // set up BSTM space trees
  int num_bstm_space_trees = 512 * 32;
  blk->new_buffer(num_bstm_space_trees * space_tree_size, 2);
  std::vector<unsigned char> &bstm_space_buffer = blk->get_buffer(2);
  space_tree_b *trees = &reinterpret_cast<space_tree_b &>(bstm_space_buffer[0]);
  // for first 16 time intervals, refine top corner (0,0,0,t) of scene. For
  // rest
  // of time, keep it unrefined.
  int ptr = 0;
  for (int i = 0; i < bstm_space_buffer.size() / space_tree_size; ++i) {
    boct_bit_tree current_tree(trees[i]);
    current_tree.set_data_ptr(ptr);
    if (i < 16) {
      current_tree.set_bit_and_parents_to_true(9);
    }
    ptr += current_tree.num_cells();
  }

  // set up BSTM time trees
  int num_bstm_time_trees = ptr;
  blk->new_buffer(num_bstm_time_trees * time_tree_size, 3);
  time_tree_b *bstm_time_trees =
      &reinterpret_cast<time_tree_b &>(blk->get_buffer(3)[0]);
  for (int i = 0; i < num_bstm_time_trees; ++i) {
    bstm_time_tree(bstm_time_trees[i]).set_data_ptr(i);
  }

  // set up data elements
  block_data_base alpha(num_bstm_time_trees, std::string("alpha"));
  block_data_base appearance(num_bstm_time_trees, std::string("bstm_mog3_grey"));

  std::vector<bool> diffs =
      dispatch_time_differences_from_bstm_trees(bstm_time_trees,
                                                trees,
                                                num_regions,
                                                &alpha,
                                                &appearance,
                                                "bstm_mog3_grey",
                                                0.01,
                                                0.01);

  compute_trees_structure(blk.ptr(), num_regions, diffs);

  // The structure of the scene should now be as follows:
  // At the top level, 1 space tree. The space tree should have only the 0'th
  // leaf voxel refined, and and its parents.
  // That leaf voxel should have a time tree
  // divided into two sections: [0,16) and [16,32).

  TEST("compute_trees_structure -- top level only has one region",
       volume(num_regions),
       1);

  boct_bit_tree top_tree(&blk->get_buffer(0)[0]);
  boct_bit_tree new_tree;
  new_tree.set_bit_and_parents_to_true(9);
  TEST("compute_trees_structure -- top tree has only corner refined",
       boct_bit_tree::same_structure(top_tree, new_tree) &&
           (top_tree.get_data_ptr() == 0),
       true);

  bool tt_good = true;
  array_4d<time_tree_b> time_trees(
      reinterpret_cast<time_tree_b *>(&(blk->get_buffer(1)[0])), 8, 8, 8, 1);
  for (int i = 0; i < time_trees.size(); ++i) {
    bstm_time_tree tt(time_trees[i]);
    std::vector<int> leaves = tt.get_leaf_bits();
    if (i == 0) {
      tt_good &= (leaves.size() == 2);
      tt_good &= (leaves[0] == 1 && leaves[1] == 2);
    } else {
      tt_good &= (leaves.size() == 1);
      tt_good &= (leaves[0] == 0);
    }
  }
  TEST("compute_trees_structure --  time trees above BSTM levels are correct",
       tt_good,
       true);

  /* TEST COALESCE TREES  */
  // now we need actual data buffers
  bstm_time_tree last_bstm_tt(bstm_time_trees[num_bstm_time_trees - 1]);
  std::size_t num_elements =
      last_bstm_tt.get_data_ptr() + last_bstm_tt.num_leaves();
  std::map<std::string, block_data_base *> datas;
  datas["alpha"] = new block_data_base(num_elements, std::string("alpha"));
  datas["bstm_mog3_grey"] =
      new block_data_base(num_elements, std::string("bstm_mog3_grey"));
  coalesce_trees(blk.ptr(), datas, num_regions, "bstm_mog3_grey");

  // test first level - space trees
  std::vector<int> top_time_trees_idxs;
  std::vector<int> bstm_space_trees_idxs;
  std::vector<int> bstm_time_trees_idxs;
  {
    std::vector<unsigned char> &top_space_buff = blk->get_buffer(0);
    TEST("coalesce -- top space buffer has one tree",
         top_space_buff.size(),
         space_tree_size);
    boct_bit_tree top_space_tree(
        reinterpret_cast<space_tree_b &>(top_space_buff[0]));
    std::vector<int> top_space_leaves = top_space_tree.get_leaf_bits();
    // test leaf indices
    bool top_space_level_good = true;
    // correct data ptr
    top_space_level_good &= (top_space_tree.get_data_ptr() == 0);
    // only first 8 leaf voxels refined
    for (int top_space_leave : top_space_leaves) {
      if (top_space_leave < 73) {
        continue;
      } else {
        top_space_level_good &= (top_space_leave < 73 + 8);
        top_time_trees_idxs.push_back(top_space_leave - 73);
      }
    }
    TEST("coalesce - top space buffer tree is correct",
         top_space_level_good,
         true);
  }

  // test second level - time trees
  {
    bool top_time_level_good = true;
    std::vector<unsigned char> &top_time_buff = blk->get_buffer(1);
    TEST("coalesce -- top time buffer has correct number of trees",
         top_time_buff.size(),
         top_time_trees_idxs.size() * time_tree_size);
    int child_ptr = 0;
    for (int tt_idx = 0; tt_idx < top_time_trees_idxs.size(); ++tt_idx) {
      int tt_ptr = top_time_trees_idxs[tt_idx];
      bstm_time_tree top_time_tree(reinterpret_cast<time_tree_b &>(
          top_time_buff[tt_ptr * time_tree_size]));
      std::vector<int> top_time_leaves = top_time_tree.get_leaf_bits();
      if (tt_idx == 0) {
        top_time_level_good &= (top_time_leaves.size() == 2);
      } else {
        top_time_level_good &= (top_time_leaves.size() == 1);
      }
      top_time_level_good &= (child_ptr == top_time_tree.get_data_ptr());
      child_ptr += top_time_leaves.size();
      // add data ptr's of children (correspond to BSTM Space Trees)
      for (int top_time_leave : top_time_leaves) {
        bstm_space_trees_idxs.push_back(
            top_time_tree.get_data_index(top_time_leave));
      }
    }
    TEST("coalesce -- top time buffer trees are correct",
         top_time_level_good,
         true);
  }

  // test BSTM space level trees
  {
    bool bstm_space_level_good = true;
    std::vector<unsigned char> &bstm_space_buff = blk->get_buffer(2);
    TEST("coalesce -- BSTM space buffer has correct number of trees",
         bstm_space_buff.size(),
         bstm_space_trees_idxs.size() * space_tree_size);
    int child_ptr = 0;
    for (int st_idx = 0; st_idx < bstm_space_trees_idxs.size(); ++st_idx) {
      int st_ptr = bstm_space_trees_idxs[st_idx];
      boct_bit_tree bstm_space_tree(reinterpret_cast<space_tree_b &>(
          bstm_space_buff[st_ptr * space_tree_size]));
      // get leaf indices -- we want to skip leaves that aren't on bottom
      // level
      std::vector<int> bstm_space_leaf_voxels;
      {
        std::vector<int> bstm_space_leaves = bstm_space_tree.get_leaf_bits();
        for (int bstm_space_leave : bstm_space_leaves) {
          if (bstm_space_leave >= 73) {
            bstm_space_leaf_voxels.push_back(bstm_space_leave);
          }
        }
      }
      if (st_idx == 0) {
        bstm_space_level_good &= (bstm_space_leaf_voxels.size() == 8);
      } else {
        bstm_space_level_good &= (bstm_space_leaf_voxels.size() == 0);
      }
      bstm_space_level_good &= (bstm_space_tree.get_data_ptr() == child_ptr);
      child_ptr += bstm_space_leaf_voxels.size();
      // add data ptr's of children (correspond to BSTM Time Trees)
      for (int i = 0; i < bstm_space_leaf_voxels.size(); ++i) {
        bstm_time_trees_idxs.push_back(bstm_space_tree.get_data_ptr() + i);
      }
    }
    TEST("coalesce -- bstm space level's trees are correct",
         bstm_space_level_good,
         true);
  }

  // test BSTM time level trees
  {
    bool bstm_time_level_good = true;
    std::vector<unsigned char> &bstm_time_buff = blk->get_buffer(3);
    TEST("coalesce -- BSTM timebuffer has correct number of trees",
         bstm_time_buff.size(),
         bstm_time_trees_idxs.size() * time_tree_size);

    for (int tt_idx = 0; tt_idx < bstm_time_trees_idxs.size(); ++tt_idx) {
      int tt_ptr = bstm_time_trees_idxs[tt_idx];
      bstm_time_tree bstm_current_tt(reinterpret_cast<time_tree_b &>(
          bstm_time_buff[tt_ptr * time_tree_size]));
      // get leaf
      std::vector<int> bstm_time_leaves = bstm_current_tt.get_leaf_bits();
      bstm_time_level_good &= (bstm_time_leaves.size() == 1);
      bstm_time_level_good &= (bstm_current_tt.get_data_ptr() == tt_idx);
    }
    TEST("coalesce -- bstm time level's trees are correct",
         bstm_time_level_good,
         true);
  }

  // test data elements
  {
    TEST("coalesce -- appropriate number of data elements",
         datas["alpha"]->buffer_length(),
         bstm_time_trees_idxs.size() *
             bstm_data_traits<BSTM_ALPHA>::datasize());
    TEST("coalesce -- appropriate number of data elements",
         datas["bstm_mog3_grey"]->buffer_length(),
         bstm_time_trees_idxs.size() *
             bstm_data_traits<BSTM_MOG3_GREY>::datasize());
  }

  // display scene size
  for (int i = 0; i < blk->buffers().size(); ++i) {
    std::cout << "level " << i << ": " << blk->get_buffer(i).size() << std::endl;
  }
  std::cout << "original number of BSTM space trees: " << num_bstm_space_trees
           << std::endl;
  std::cout << "original number of BSTM time trees: " << num_bstm_time_trees
           << std::endl;
  std::cout << "alpha bytes: "
           << num_bstm_time_trees * bstm_data_info::datasize("alpha")
           << " became: " << datas["alpha"]->buffer_length() << std::endl;
  std::cout << "appearance bytes: "
           << num_bstm_time_trees * bstm_data_info::datasize("bstm_mog3_grey")
           << " became: " << datas["bstm_mog3_grey"]->buffer_length()
           << std::endl;
}

void test_fly_problem() {
  /*  set up Multi-BSTM block  */
  std::vector<space_time_enum> subdivs(4);
  subdivs[0] = STE_SPACE;
  subdivs[1] = STE_TIME;
  subdivs[2] = STE_SPACE;
  subdivs[3] = STE_TIME;
  bstm_multi_block_metadata mdata(bstm_block_id(),
                                  vgl_box_3d<double>(0, 0, 0, 1, 1, 1),
                                  std::pair<double, double>(0, 1),
                                  9000,
                                  0.01,
                                  subdivs);
  bstm_multi_block_sptr blk = new bstm_multi_block(mdata);

  /* Set up bstm blocks */
  bstm_block_metadata bstm_mdata(
      bstm_block_id(),
      vgl_point_3d<double>(0, 0, 0),
      0,
      vgl_vector_3d<double>(1 / 8.0, 1 / 8.0, 1 / 8.0),
      1 / 32.0,
      vgl_vector_3d<unsigned>(8, 8, 8),
      32,
      1,
      4,
      9000,
      .01);

  int num_space_trees = 8 * 8 * 8;
  char *bstm_space_buff = new char[num_space_trees * space_tree_size]();
  bstm_block_sptr bstm_blk =
      new bstm_block(bstm_mdata.id(), bstm_mdata, bstm_space_buff);

  // temporary buffers that we grow as necessary, and then copy into final
  // buffers
  std::vector<time_tree_b> bstm_time_trees;
  std::vector<bstm_data_traits<BSTM_ALPHA>::datatype> alpha;
  std::vector<bstm_data_traits<BSTM_MOG3_GREY>::datatype> appearance;

  // refine space trees - refine a point diagonally across each space tree
  // over
  // time
  boxm2_array_3d<space_tree_b> bstm_space_trees(
      8, 8, 8, reinterpret_cast<space_tree_b *>(bstm_space_buff));

  for (int space_idx = 0; space_idx < bstm_space_trees.size(); ++space_idx) {
    // check if tree is along diagonal
    std::size_t x, y, z;
    bstm_space_trees.coords_from_index(space_idx, x, y, z);
    boct_bit_tree current_tree(bstm_space_trees(x, y, z));
    current_tree.set_data_ptr(bstm_time_trees.size() / 32);
    if (x == y && y == z) {
      int i = x;
      // leaf idxs of refined points - these will actually have non-empty time
      // trees
      std::vector<int> refined_idxs;
      // as it happens all these indexes are in increasing order.
      for (int j = 0; j < 8; ++j) {
        int idx = current_tree.traverse(
            vgl_point_3d<double>(j / 8.0, j / 8.0, j / 8.0), 4, true);
        current_tree.set_bit_and_parents_to_true(
            current_tree.parent_index(idx));
        refined_idxs.push_back(idx);
      }

      // set up time trees
      std::vector<int>::const_iterator idxs_iter = refined_idxs.begin();
      std::vector<int> cell_bits = current_tree.get_cell_bits();
      for (int cell : cell_bits) {
        bool is_refined_cell = false;
        int cell_pos; // position in space tree of cell, along diagonal, in
                      // [0,8).
        if (idxs_iter != refined_idxs.end() && cell == *idxs_iter) {
          cell_pos = std::distance(static_cast<std::vector<int>::const_iterator>(
                                      refined_idxs.begin()),
                                  idxs_iter);
          ++idxs_iter;
          is_refined_cell = true;
        }
        // create 32 time trees for cell
        for (int tt_idx = 0; tt_idx < 32; ++tt_idx) {
          bstm_time_trees.emplace_back((unsigned char)0);
          bstm_time_tree tt(bstm_time_trees.back());
          tt.set_data_ptr(alpha.size());

          // check if this time tree should be refined
          bool is_refined_tt =
              is_refined_cell && ((i * 8 + cell_pos) / 2 == tt_idx);
          bool frames[32] = {};
          frames[0] = frames[1] = frames[16] = frames[17] = is_refined_tt;
          tt.fill_cells(frames);

          // now create data elements accordingly
          std::vector<int> tt_leaves = tt.get_leaf_bits();
          for (std::vector<int>::const_iterator tt_leaf_iter = tt_leaves.begin();
               tt_leaf_iter != tt_leaves.end();
               ++tt_leaf_iter) {
            int frame = *tt_leaf_iter - 31;

            // data element is non-zero if it corresponds to a refined point
            bool is_refined_frame =
                is_refined_tt && (frame == 0 || frame == 16);
            alpha.push_back(is_refined_frame ? 1.0 : 0.0);
            appearance.emplace_back(
                (unsigned char)(is_refined_frame ? 128 : 0));
          }
        }
      }
    } else {
      // if not along diagonal, make this tree empty and add 32 time trees
      // & data elements.
      for (int t = 0; t < 32; ++t) {
        bstm_time_trees.emplace_back((unsigned char)0);
        bstm_time_tree(bstm_time_trees.back()).set_data_ptr(alpha.size());
        alpha.push_back(0.0);
        appearance.emplace_back((unsigned char)0);
      }
    }
  }

  // copy time tree and data buffers
  std::size_t time_buff_size = bstm_time_trees.size() * time_tree_size;
  char *time_buffer = new char[time_buff_size];
  std::memcpy(time_buffer, &(bstm_time_trees[0]), time_buff_size);
  bstm_time_block_sptr bstm_blk_t = new bstm_time_block(
      bstm_block_id(), bstm_mdata, time_buffer, time_buff_size);

  std::size_t alpha_buff_size =
      alpha.size() * bstm_data_traits<BSTM_ALPHA>::datasize();
  char *alpha_buffer = new char[alpha_buff_size];
  std::memcpy(alpha_buffer, &(alpha[0]), alpha_buff_size);

  std::size_t appearance_buff_size =
      appearance.size() * bstm_data_traits<BSTM_MOG3_GREY>::datasize();
  char *appearance_buffer = new char[appearance_buff_size];
  std::memcpy(appearance_buffer, &(appearance[0]), appearance_buff_size);

  std::map<std::string, bstm_data_base *> bstm_datas;
  std::map<std::string, block_data_base *> datas;

  bstm_datas["alpha"] =
      new bstm_data_base(alpha_buffer, alpha_buff_size, bstm_blk->block_id());
  bstm_datas["bstm_mog3_grey"] = new bstm_data_base(
      appearance_buffer, appearance_buff_size, bstm_blk->block_id());
  datas["alpha"] = new block_data_base(0);
  datas["bstm_mog3_grey"] = new block_data_base(0);

  std::cout << "********** " << bstm_space_trees.size() << ", "
           << bstm_time_trees.size() << ", " << alpha.size() << ", "
           << appearance.size() << std::endl;

  bstm_block_to_bstm_multi_block(blk.ptr(),
                                 datas,
                                 bstm_blk.ptr(),
                                 bstm_blk_t.ptr(),
                                 bstm_datas,
                                 0.01,
                                 0.01);

  std::cout << "********** " << std::endl;
  for (int i = 0; i < blk->buffers().size(); ++i) {
    std::cout << blk->get_buffer(i).size() /
                    tree_size(blk->metadata().subdivisions_[i])
             << std::endl;
  }
  std::cout << datas["alpha"]->buffer_length() /
                  bstm_data_traits<BSTM_ALPHA>::datasize()
           << std::endl;
}

std::pair<bstm_data_traits<BSTM_ALPHA>::datatype,
         bstm_data_traits<BSTM_MOG3_GREY>::datatype>
fly_scene_generate(const index_4d &coords) {
  if (coords[0] == coords[1] && coords[1] == coords[2]) {
    if (coords[3] % 16 == 0 && coords[3] / 16 == coords[0]) {
      std::cout << coords << std::endl;
      return std::make_pair(bstm_data_traits<BSTM_ALPHA>::datatype(1), bstm_data_traits<BSTM_MOG3_GREY>::datatype((unsigned char)128));
    }
  }
  return std::make_pair(bstm_data_traits<BSTM_ALPHA>::datatype(0), bstm_data_traits<BSTM_MOG3_GREY>::datatype((unsigned char)0));
}

void test_fly_problem2() {
  /*  set up Multi-BSTM block  */
  std::vector<space_time_enum> subdivs(4);
  subdivs[0] = STE_SPACE;
  subdivs[1] = STE_TIME;
  subdivs[2] = STE_SPACE;
  subdivs[3] = STE_TIME;
  bstm_multi_block_metadata mdata(bstm_block_id(),
                                  vgl_box_3d<double>(0, 0, 0, 1, 1, 1),
                                  std::pair<double, double>(0, 1),
                                  9000,
                                  0.01,
                                  subdivs);
  bstm_multi_block_sptr blk = new bstm_multi_block(mdata);
  std::map<std::string, block_data_base *> datas;
  datas["alpha"] = new block_data_base(0);
  datas["bstm_mog3_grey"] = new block_data_base(0);

  bstm_block_metadata bstm_mdata(
      bstm_block_id(),
      vgl_point_3d<double>(0, 0, 0),
      0,
      vgl_vector_3d<double>(1 / 8.0, 1 / 8.0, 1 / 8.0),
      1 / 32.0,
      vgl_vector_3d<unsigned>(8, 8, 8),
      32,
      1,
      4,
      9000,
      .01);

  vbl_triple<bstm_block *,
             bstm_time_block *,
             std::map<std::string, bstm_data_base *> >
      bstm_values = bstm_scene_from_point_func<BSTM_ALPHA, BSTM_MOG3_GREY>(
          bstm_mdata, fly_scene_generate);

  std::cout << "********** " << bstm_values.first->trees().size() << ", "
           << bstm_values.second->time_trees().size() << ", "
           << bstm_values.third["alpha"]->buffer_length() << std::endl;

  bstm_block_to_bstm_multi_block(blk.ptr(),
                                 datas,
                                 bstm_values.first,
                                 bstm_values.second,
                                 bstm_values.third,
                                 0.01,
                                 0.01);

  std::cout << "********** " << std::endl;
  for (int i = 0; i < blk->buffers().size(); ++i) {
    std::cout << blk->get_buffer(i).size() /
                    tree_size(blk->metadata().subdivisions_[i])
             << std::endl;
  }
  std::cout << datas["alpha"]->buffer_length() /
                  bstm_data_traits<BSTM_ALPHA>::datasize()
           << std::endl;
}

void test_bstm_to_multi_bstm_block_function() {
  // test implementation sub-routines...
  test_volume();
  test_get_bstm_data_buffers();
  test_convert_bstm_space_trees();
  test_time_differences_from_bstm_trees();
  test_make_unrefined_space_tree();
  test_make_unrefined_time_tree();
  test_compute_and_coalesce_trees();

  // test empty scene

  // test actual BSTM scene
  test_fly_problem();
  // test_fly_problem2();
}

TESTMAIN(test_bstm_to_multi_bstm_block_function);
